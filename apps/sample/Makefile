cpu:
	g++ -o sample sample.cpp -DSAMPLE_CPU -I$(SNUCLROOT)/inc -lOpenCL

single:
	g++ -o sample sample.cpp -DSAMPLE_SINGLE -I$(SNUCLROOT)/inc -lOpenCL

cluster:
	mpic++ -o sample sample.cpp -DSAMPLE_CLUSTER -I$(SNUCLROOT)/inc -L$(SNUCLROOT)/lib -lsnucl_cluster
