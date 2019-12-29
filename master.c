int master(void)
{
    MPI_Comm_set_errhandler(comm, MPI_ERRORS_RETURN);
    MPI_Comm_size(comm, &size);

    /* ... submit the initial work requests ... */

    MPI_Irecv( buffer, 1, MPI_INT, MPI_ANY_SOURCE, tag, comm, &req );

    /* 
     *    * Progress engine: Get answers, send new requests, 
     *       * and handle process failures 
     *          */
    while( (active_workers > 0) && work_available ) {
        rc = MPI_Wait( &req, &status );

        if( (MPI_ERR_PROC_FAILED == rc) || (MPI_ERR_PENDING == rc) ) {
            MPI_Comm_failure_ack(comm);
            MPI_Comm_failure_get_acked(comm, &g);
            MPI_Group_size(g, &gsize);

            /* ... find the lost work
             * and requeue it ... */

            active_workers = size - gsize - 1;
            MPI_Group_free(&g);

            /* repost the
             * request if it
             * matched the
             * failed
             * process */
            if( rc == MPI_ERR_PROC_FAILED ) {
                MPI_Irecv( buffer, 1, MPI_INT, MPI_ANY_SOURCE,
                        tag, comm, &req );
            }

            continue;
        }

        /* ... process the answer and update work_available ... */
        MPI_Irecv( buffer, 1, MPI_INT, MPI_ANY_SOURCE, tag, comm, &req );
    }

    /* ... cancel request and cleanup ... */
}
