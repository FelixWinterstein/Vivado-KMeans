/**********************************************************************
* Felix Winterstein, Imperial College London
*
* File: tb_io.cpp
*
* Revision 1.01
* Additional Comments: distributed under a BSD license, see LICENSE.txt
*
**********************************************************************/

#include "tb_io.h"

#define mytype short int

void make_data_points_file_name(char *result, uint n, uint k, uint d, double std_dev)
{
    sprintf(result,"data_points_N%d_K%d_D%d_s%.2f.mat",n,k,d,std_dev);
}

void make_initial_centres_file_name(char *result, uint n, uint k, uint d, double std_dev, uint index)
{
    sprintf(result,"initial_centres_N%d_K%d_D%d_s%.2f_%d.mat",n,k,d,std_dev,index);
}


void make_clustering_centres_file_name(char *result, uint n, uint k, uint d, double std_dev, uint index)
{
    sprintf(result,"output_data/clustering_centres_N%d_K%d_D%d_s%.2f_%d.mat",n,k,d,std_dev,index);
}


bool read_data_points(uint n, uint k, double std_dev, data_type* points, uint* index)
{
    FILE *fp;
    char filename[256];
    make_data_points_file_name(filename,n,k,D,std_dev);
    fp=fopen(filename, "r");

    if (fp == 0) {
        printf("failed to open file\n");
        return false;
    }
    char tmp[16];

    for (uint j=0; j<D; j++) {
        for (uint i=0;i<n;i++) {
            if (fgets(tmp,16,fp) == 0) {
                fclose(fp);
                return false;
            } else {
                //printf("%s\n",tmp);
                //points[i].value[j]=(mytype)atoi(tmp); // assume coord_type==short int
                coord_type b;
                b.VAL = (mytype)atoi(tmp); // assume coord_type==short int
                set_coord_type_vector_item(&points[i].value, b, j);
            }
        }
    }

    for (uint i=0;i<n;i++) {
        *(index+i) = i;
    }

    fclose(fp);

    return true;
}


bool read_initial_centres(uint n, uint k, double std_dev, data_type *initial_centre_positions, uint* centr_idx)
{
    FILE *fp;
    char filename[256];
    make_initial_centres_file_name(filename,n,k,D,std_dev,FILE_INDEX);
    fp=fopen(filename, "r");
    if (fp == 0) {
        printf("failed to open file\n");
        return false;
    }
    char tmp[16];

    for (uint j=0; j<D; j++) {
        for (uint i=0;i<k;i++) {
            if (fgets(tmp,16,fp) == 0) {
                fclose(fp);
                return false;
            } else {
                //printf("%s\n",tmp);
                //initial_centre_positions[i].value[j] = (mytype)atoi(tmp); // assume coord_type==short int
                coord_type b;
                b.VAL = (mytype)atoi(tmp); // assume coord_type==short int
                set_coord_type_vector_item(&initial_centre_positions[i].value, b, j);
            }
        }
    }

    fclose(fp);

    return true;
}



/********************* debug IO ************************************/



void print_data_type_array(data_type *c, centre_index_type k)
{
    for (centre_index_type i=0; i<=k; i++) {
        for (uint d=0; d<D-1; d++) {
            coord_type tmp = get_coord_type_vector_item(c[i].value,d);
            printf("%d ",tmp.VAL);
        }
        coord_type_ext tmp = get_coord_type_vector_item(c[i].value,D-1);
        printf("%d\n",tmp.VAL);
    }
}


void print_centre_array(centre_type *c, centre_index_type k)
{
    for (centre_index_type i=0; i<=k; i++) {
        for (uint d=0; d<D-1; d++) {
            coord_type_ext tmp = get_coord_type_vector_ext_item(c[i].wgtCent.value,d);
            printf("%d ",tmp.VAL);
        }
        coord_type_ext tmp = get_coord_type_vector_ext_item(c[i].wgtCent.value,D-1);
        printf("%d\n",tmp.VAL);
    }
}



