#include <syslog.h>
#include <stdio.h>

int main(int argc, char *argv[]){
    //Start the logger
    openlog(NULL, 0, LOG_USER);

    if(argc-1 == 2){
        char *writefile = argv[1];
        char *writestr = argv[2];
        FILE *new_file = fopen(writefile, "w");
        fprintf(new_file, "%s", writestr);
        fclose(new_file);

        printf("Writing %s to %s\n", writestr, writefile);
        //Log the success
        syslog(LOG_DEBUG, "Writing %s to %s", writestr, writefile);
        closelog();
        return 0;
    } else {
        printf("Error: Too few arguments, (%i given) %i required\n", argc-1, 2);

        //Log an error
        syslog(LOG_ERR, "Error: Too few arguments, (%i given) %i required", argc-1, 2);
        closelog();
        return 1;
    }
}