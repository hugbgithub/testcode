#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <vector>
#include <stdlib.h>

#define FILEPATH_MAX 250

using namespace std;

void get_dir(char *dir, vector<char*> &vec)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if((dp = opendir(dir)) == NULL) {
        fprintf(stderr,"cannot open directory: %s\n", dir);
        return;
    }
    chdir(dir);
    char *curr_path = (char *)malloc(FILEPATH_MAX);
    getcwd(curr_path,FILEPATH_MAX);
    vec.push_back(curr_path);
    while((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name,&statbuf);
        if(S_ISDIR(statbuf.st_mode)) {
           
            if(strcmp(".",entry->d_name) == 0 ||
                strcmp("..",entry->d_name) == 0)
                continue;
            char *tmp = (char*)malloc(strlen(entry->d_name)+1);
            memset(tmp, 0x00, strlen(entry->d_name)+1);
            strcpy(tmp, entry->d_name);
      
            get_dir(entry->d_name, vec);
        }
        
    }
    chdir("..");
    closedir(dp);
}
void printdir(char *dir, int depth)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if((dp = opendir(dir)) == NULL) {
        fprintf(stderr,"cannot open directory: %s\n", dir);
        return;
    }
    chdir(dir);
    while((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name,&statbuf);
        if(S_ISDIR(statbuf.st_mode)) {
           
            if(strcmp(".",entry->d_name) == 0 ||
                strcmp("..",entry->d_name) == 0)
                continue;
            printf("%d%s%s/\n",depth,"",entry->d_name);
           
            printdir(entry->d_name,depth+4);
        }
        else printf("%d%s%s\n",depth,"",entry->d_name);
    }
    chdir("..");
    closedir(dp);
}

int main(int argc, char* argv[])
{
    char *topdir, pwd[2]=".";
    if (argc != 2)
        topdir=pwd;
    else
        topdir=argv[1];
    printf("Directory scan of %s\n",topdir);
    
    vector<char*> vec;
    vector<char*>::iterator iter;

    get_dir(topdir, vec);
    for(iter = vec.begin(); iter != vec.end(); iter++){
      printf("%s\n", *iter);
      
    }

    
    
    printf("done.\n");
    return 0;
}
