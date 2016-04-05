int test(){
    int a[1],i,x[1];
    int *b;
    b=x;
    for(i=0;i<1;i++){
        a[i] = i*i;
        *(b+i) = a[i]+i;
    }

    return a[0];
}
