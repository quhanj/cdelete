#ifndef CDELETE_H
#define CDELETE_H
enum cdeletetype{
	CDBUF,CDOPEN,CDFOP,CDJSON,CDPQC,CDPQR,CDCURL
};
#define CDELETE(p,t) cdelete_realfunction((void*)&(p),(t))
void cdelete_realfunction(void** p,enum cdeletetype t);
#endif