#include <stdlib.h>
#include <stdio.h>

int my_strcmp (char * first , char * second){
    int i = 0;
    while(first[i] != '\0'){
        if (second[i] == '\0')
            return -1;
        if (first[i] != second[i])
            return -1;
        i++;
    }
    if (second[i] == '\0')
        return 0;
    else
        return -1;

}

int my_strncmp (char * first , char * second , int n){
    int i = 0;
    while(i<n){
        if (second[i] == '\0' && first[i] == '\0')
            return 0;
        if (second[i] == '\0' || first[i] == '\0')
            return -1;
        if (first[i] != second[i])
            return -1;
        i++;
    }
    return 0;
}

int main(int argc, char **argv) {
	int state = 0; //0 is normal state, 1 is add state, 2 is subtract state.
	int debug = 0;
	int earg = 0; //indicates the e argumant.
	char input = 0;
	int pos = 0;
    FILE *infile = stdin;
	FILE *outfile = stdout;
        for (int i=1;i<argc;i++){
		if (my_strcmp(argv[i],"+D")==0){
			debug = 1;
		}
		if (my_strcmp(argv[i], "-D")==0){
			debug = 0;
		}
		if (my_strncmp(argv[i],"+e",2)==0){
			state = 1;
			earg = i;
		}
		if (my_strncmp(argv[i],"-e",2)==0){
			state = 2;
			earg = i;
		}
		if (my_strncmp(argv[i],"-i",2)==0){
			infile = fopen(argv[i]+2,"r");
			if (infile == NULL){
			    fprintf(stderr,"couldn't open the input file\n");exit(0);
			}	
		}
		if (my_strncmp(argv[i],"-o",2)==0){
			outfile = fopen(argv[i]+2,"w+");
			if (infile == NULL){
				fprintf(stderr,"couldn't open the input file\n");exit(0);
			}
		}
		if (debug){
			fprintf(stderr,"%s",argv[i]);
			fprintf(stderr,"\n");
		}
        }
	while((input=fgetc(infile))!=EOF){
		if (state==1){ //add State
			if (input>='0'&&input<='9'){
				(input+argv[earg][2+pos]-'0')>'9'?fputc(((input-'0')+(argv[earg][2+pos]-'0'))%10+'0',outfile):fputc(input+argv[earg][2+pos]-'0',outfile);
			}
			else if (input>='a'&&input<='z'){
				(input+argv[earg][2+pos]-'0')>'z'?fputc(((input-'a')+(argv[earg][2+pos]-'0'))%26+'a',outfile):fputc(input+argv[earg][2+pos]-'0',outfile);
			}
			else if (input>='A'&&input<='Z'){
				(input+argv[earg][2+pos]-'0')>'Z'?fputc(((input-'A')+(argv[earg][2+pos]-'0'))%26+'A',outfile):fputc(input+argv[earg][2+pos]-'0',outfile);
			}
			else
				fputc(input,outfile);
		
		}
		else if (state==2){ //subtract state
			if (input>='0'&&input<='9'){
				(input-argv[earg][2+pos]-'0')<'0'?fputc(((input-'0')-(argv[earg][2+pos]-'0')+10)%10+'0',outfile):fputc(input-argv[earg][2+pos]-'0',outfile);
			}
			else if (input>='a'&&input<='z'){
				(input-argv[earg][2+pos]-'0')<'a'?fputc(((input-'a')-(argv[earg][2+pos]-'0')+26)%26+'a',outfile):fputc(input-argv[earg][2+pos]-'0',outfile);
			}
			else if (input>='A'&&input<='Z'){
				(input-argv[earg][2+pos]-'0')<'A'?fputc(((input-'A')-(argv[earg][2+pos]-'0')+26)%26+'A',outfile):fputc(input-argv[earg][2+pos]-'0',outfile);
			}
			else 
				fputc(input,outfile);

		}
		else 
			fputc(input,outfile);
		pos++;
		argv[earg][2+pos]=='\0'?pos=0:1;
		fflush(outfile);
	}	
        fprintf(stderr, "\n");
  return 0;
}                                                                                                                                                                                                                                              


