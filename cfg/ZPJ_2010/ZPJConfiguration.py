
class section(object):
    def __init__(self,name,**parameters):
        self.__name=name
        self.__parameters = parameters
        for key,val in self.__parameters.items():
            self.__dict__[key]=val
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
        self.__name = name
        self.sections_names=[]
    #---------------------------------------------
    def update(self):
        s=section('test')
        self.sections_names=[]
        for key,val in self.__dict__.items():
            if type(s)==type(val):
                self.sections_names.append(val.getName())
    #---------------------------------------------
    def dump_cfg(self):
        s=section('test')
        for key,val in self.__dict__.items():
            if type(s)==type(val):
                print val.dump_cfg()
        print '[__internal__]'
        print 'sections = "%s"' %self.sections_names[0],
        for section_name in self.sections_names[1:]:
              print ',"%s"' %section_name,
    #---------------------------------------------
    def setName(self,name):
        self.__name=name
