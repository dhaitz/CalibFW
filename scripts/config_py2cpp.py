#! /usr/bin/python
# -*- coding: utf-8 -*-

class section(object):
    def __init__(self,name,**parameters):                                                               
        self.__name=name                                                                                
        self.__parameters = parameters                                                                  
        for key,val in self.__parameters.items():                                                       
            self.__dict__[key]=val                                                                      
    #---------------------------------------------                                                      
    def getParameters(self):
	return self.__parameters
    #---------------------------------------------                                                      
    def setName(self,name):                                                                             
        self.__name=name                                                                                
    #---------------------------------------------                                                      
    def getName(self):                                                                                  
        return self.__name                                                                              
    #---------------------------------------------                                                      
                                                                                                        
    def dump_cfg(self):                                                                                 
        print '[%s]' %self.__name                                                                       
        for key,val in self.__dict__.items():                                                           
            if (key[0]!='_'):                                                                           
                if type(val)==type(1) or type(val)==type(1.1):
                    print '    %s = %s' %(key,val)
                elif type(val)==type(''):
                    print '    %s = "%s"' %(key,val)
                elif type(val)==type([]):
                    sep=''
                    if type(val[0])==type(''):
                        sep='"'
                    print '    %s = ' %key,
                    liststring=''
                    for el in val:
                        liststring+='%s%s%s,' %(sep,el,sep)
                    liststring=liststring[:-1]
                    print liststring
                else:
                    continue
        print '#'+'-'*79
#-------------------------------------------------------------------------------
class configuration(object):

    def __init__(self,name):
        self.__name=name
    #---------------------------------------------
    def dump_cfg(self):
        s=section('test')
        for key,val in self.__dict__.items():
            if type(s)==type(val):
                print val.dump_cfg()
    #---------------------------------------------
    def dump_cpp(self):
        s=section('test')
        for key,val in self.__dict__.items():
            if type(s)==type(val):
		print "\n//\n// Section %s\n//" %val.getName()
		for par_name,par_val in val.getParameters().items():
		    if type([])!=type(par_val): # we do not have a list
			if type('ciao') == type(par_val): #we have a string
			    print 'TString %s = p.getString(title+".%s");' %(par_name,par_name)
			elif type(1.2) == type(par_val): #we have a double
			    print 'double %s = p.getDouble(title+".%s");' %(par_name,par_name)
			else: #we have an int
			    print 'int %s = p.getInt(title+".%s");' %(par_name,par_name)
		    else: # we have an array
			first_el=par_val[0]
			if type('ciao') == type(first_el): #we have a string
			    print 'vString %s = p.getvString(title+".%s");' %(par_name,par_name)
			elif type(1.2) == type(first_el): #we have a double
			    print 'vdouble %s = p.getvDouble(title+".%s");' %(par_name,par_name)
			else: #we have an int
			    print 'vint %s = p.getvInt(title+".%s");' %(par_name,par_name)
    #---------------------------------------------
    def setName(self,name):
        self.__name=name
#-------------------------------------------------------------------------------

import sys

argv = sys.argv
argc = len(argv)

if (argc!=2):
    print argv[0]+" configfilename"
    sys.exit(1)

execfile(argv[1])
print '// ------> Replace "title" by the section name <------'
exec("p.dump_cpp()")
