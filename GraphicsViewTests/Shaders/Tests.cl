void
kernel Tests()
{
    int g = get_global_id( 0 );
    int l = get_local_id( 0 );
    printf("Group : %d, local : %d\n", g, l );
}