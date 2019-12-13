/* Fault Tolerant Manager / Worker Example
 * using notification and task spawning.
 * example1.c
 */

#include <stdio.h>
#include <math.h>
#include <pvm3.h>

#define NWORK        4
#define NPROB        10000
#define MSGTAG       123

int main()
{
    double sum = 0.0, result, input = 1.0;
    int tids[NWORK], numt, probs[NPROB], sent=0, recvd=0;
    int aok=0, cc, bufid, done=0, i, j, marker, next, src;

    /* If I am a Manager Task */
    if ( (cc = pvm_parent()) == PvmNoParent || cc == PvmParentNotSet ) {

        /* Spawn NWORK Worker Tasks */
        numt = pvm_spawn( "example1", (char **) NULL, PvmTaskDefault,
                (char *) NULL, NWORK, tids );

        /* Set Up Notify for Spawned Tasks */
        pvm_notify( PvmTaskExit, MSGTAG, numt, tids );

        /* Send Problem to Spawned Workers */
        for ( i=0 ; i < NPROB ; i++ ) probs[i] = -1;
        for ( i=0 ; i < numt ; i++ ) {
            pvm_initsend( PvmDataDefault );
            pvm_pkint( &aok, 1, 1 );  /* Valid Problem Marker */
            input = (double) (i + 1);
            pvm_pkdouble( &input, 1, 1 );
            pvm_send( tids[i], MSGTAG );
            probs[i] = i; sent++;  /* Next Problem */
        }

        /* Collect Results / Handle Failures */
        do {
            /* Receive Result */
            bufid = pvm_recv( -1, MSGTAG );
            pvm_upkint( &marker, 1, 1 );

            /* Handle Notify */
            if ( marker > 0 ) {
                /* Find Failed Task Index */
                for ( i=0, next = -1 ; i < numt ; i++ )
                    if ( tids[i] == marker )
                        /* Find Last Problem Sent to Task */
                        for ( j=(sent-1) ; j > 0 ; j-- )
                            if ( probs[j] == i ) {
                                /* Spawn Replacement Task */
                                if ( pvm_spawn( "example1", (char **) NULL,
                                        PvmTaskDefault, (char *) NULL, 1,
                                        &(tids[i]) ) == 1 ) {
                                    pvm_notify( PvmTaskExit, MSGTAG, 1,
                                            &(tids[i]) );
                                    next = i;  sent--;
                                }
                                probs[j] = -1; /* Reinsert Prob */
                                break;
                            }
            } else {
                /* Get Source Task & Accumulate Solution */
                pvm_upkdouble( &result, 1, 1 );
                sum += result;
                recvd++;
                /* Get Task Index */
                pvm_bufinfo( bufid, (int *) NULL, (int *) NULL, &src );
                for ( i=0 ; i < numt ; i++ )
                    if ( tids[i] == src ) next = i;
            }

            /* Send Another Problem */
            if ( next >= 0 ) {
                for ( i=0, input = -1.0 ; i < NPROB ; i++ )
                    if ( probs[i] < 0 ) {
                        input = (double) (i + 1);
                        probs [i] = next; sent++;  /* Next Problem */
                        break;
                    }
                pvm_initsend( PvmDataDefault );
                pvm_pkint( &aok, 1, 1 );  /* Valid Problem Marker */
                pvm_pkdouble( &input, 1, 1 );
                pvm_send( tids[next], MSGTAG );
                if ( input < 0.0 ) tids[next] = -1;
            }

        } while ( recvd < sent );

        printf( "Sum = %lf\n", sum );
    }

    /* If I am a Worker Task */
    else if ( cc > 0 ) {
        /* Notify Me If Manager Fails */
        pvm_notify( PvmTaskExit, MSGTAG, 1, &cc );
        /* Solve Problems Until Done */
        do {
            /* Get Problem from Master */
            pvm_recv( -1, MSGTAG );
            pvm_upkint( &aok, 1, 1 );
            if ( aok > 0 )  /* Master Died */
                break;
            pvm_upkdouble( &input, 1, 1 );
            if ( input > 0.0 ) {
                /* Compute Result */
                result = sqrt( ( 2.0 * input ) - 1.0 );
                /* Send Result to Master */
                pvm_initsend( PvmDataDefault );
                pvm_pkint( &aok, 1, 1 );    /* Ask for more... */
                pvm_pkdouble( &result, 1, 1 );
                pvm_send( cc, MSGTAG );
            } else
                done = 1;
        } while ( !done );
    }

    pvm_exit();

    return( 0 );
}
