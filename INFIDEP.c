/* File   INFIDEP.c */
#include "Yproto.h"
void show_time_info(YDC ydc);

main(argc, argv)
  INT argc; char **argv;
{ CHR c1name[300];         /* name of the problem i.e. input file */
  struct YD_struct yd;     /* Y database                          */
  YDC ydc=&(yd.ydc);       /* Y control database                  */
  YDE yde=&(yd.yde);       /* Y element database                  */
  YDI ydi=&(yd.ydi);       /* Y interaction database              */
  YDN ydn=&(yd.ydn);       /* Y node database                     */
  YDO ydo=&(yd.ydo);       /* Y output database                   */
  YDPE ydpe=&(yd.ydpe);    /* Y property database  for elements   */
  YDPN ydpn=&(yd.ydpn);    /* Y property database  for nodes (BC) */
  YDPJ ydpj=&(yd.ydpj);    /* Y property database  for joints     */
  YDPM ydpm=&(yd.ydpm);    /* Y property database  for meshing    */
  INT Tctrlc, itimes=0;
  
  /* process data */
  CHRw(stdout,"INFIDEP, Pro. Bo Han, Dr. Bo-nan. \n");
  CHRw(stdout,"Version 2.1 2D/3D. \n");
  CHRw(stdout,"Finite Discrete Element Method. \n");
  CHRw(stdout,"Based on Y-code. Dr. Munjiza. \n\n");
  
    
  /* get name of the problem */
  if(argv[1]!=NULL)
  { CHRcpy(c1name,argv[1]);
  }
  else
  { CHRwcr(stdout);
    CHRw(stdout,"  please define input file names: "); CHRwcr(stdout);
    CHRw(stdout," >");
    gets(c1name);
  }
  strcpy(ydc->cfiname, c1name);   ydc->cfiname[255]='\0';
  ydc->finp=FILENULL; ydc->fcheck=FILENULL;
  
  /* Process while any input */
  while(Yrd(c1name,&yd)>0)
  { itimes=itimes+1;
    CHRw(stdout,"NEW INPUT: "); CHRw(stdout, c1name); CHRwcr(stdout);
    if(Ycheck(&yd)<0) break;    date_and_time(ydc->cruntime); timestamp();
    CHRw(stdout, "Start calculating ...\n");
    for(ydc->ncstep=ydc->ncstep;ydc->ncstep<ydc->mcstep;ydc->ncstep++)
    { show_time_info(ydc);                            /* show time information    */
      Ymd(ydc,yde,ydi,ydn,ydpe, ydpn,ydpm);           /* mesh elements            */
      Yfd(ydc,yde,ydn,ydi,ydo,ydpe,ydpn,ydpj);        /* nodal forces             */
      Ybor(ydc,yde,ydn,ydb,yds,ydpe,ydpj,ydpn);       /* borholes, inter. fluid   */
      Ycd(ydc,yde,ydi,ydn,ydpe,ydpn);                 /* contact detection        */
      Yid(ydc,yde,ydi,ydn,ydo,ydpe,ydpn, ydpj,ydpm);  /* interaction              */
      Yod(c1name,&yd);                                /* output results           */
      Ysd(ydc,yde,ydn,ydo,ydpe,ydpn );                /* solve equations          */
      Yfrd(ydc,yde,ydi,ydn,ydpe,ydpn,ydpj,ydpm);      /* fracture                 */
      ydc->dctime=ydc->dctime+ydc->dcstec;            /* update time              */
      /* CTRL-C Interruption */
      Tctrlc = enablc(ydc->dctime, ydc->ncstep, ydc->mcstep);    if(Tctrlc!=1) break;
    }
  }
  
  /* Termination */
  CHRw(stderr,"   ***** INFIDEP HAS ORDERLY FINISHED *****");  CHRwcr(stderr);
  CHRw(stderr,"Press a key to continue");  CHRwcr(stderr);
  getchar();
}

void show_time_info(YDC ydc)
{ INT static itimes = 0;
  FILE *fptr=FILENULL, *fcheck=FILENULL;
  CHR  datetime[300];
  DBL runtime, ctime;
  INT ncstep, icshtf, il;
 
  fcheck = ydc->fcheck;
  ncstep = ydc->ncstep;
  ctime =  ydc->dctime;
  icshtf = ydc->icshtf;
  
  if(itimes==0)
  { itimes = 1;
    for(il=0; il<2; il++)
    { fptr=stdout; if(il==1) fptr=fcheck;
      CHRww(fptr, "Ncycle", 8); CHRww(fptr, "Physi. time", 17); 
      CHRww(fptr, "Date and Time", 22);
      CHRww(fptr, "Elapsed time", 15);
      CHRwcr(fptr);
    }
  }
 
  if(ncstep%icshtf==0)
  { for(il=0; il<2; il++)
    { fptr=stdout; if(il==1) fptr=fcheck;
      date_and_time(datetime);
      runtime = run_time();
      INTw(fptr, ydc->ncstep, 8); fprintf(fptr, "  %15.7e",ctime); CHRwsp(fptr);
      CHRwsp(fptr);  CHRwsp(fptr);  CHRw(fptr, datetime);  
      fprintf(fptr, "%15.3f", runtime);
      CHRwcr(fptr);
    }
  }
}
