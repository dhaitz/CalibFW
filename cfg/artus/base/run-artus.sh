echo $FILE_NAMES
cd $CMSSW_BASE
export VO_CMS_SW_DIR=/wlcg/sw/cms
export SCRAM_ARCH=slc5_amd64_gcc462
source $VO_CMS_SW_DIR/cmsset_default.sh
eval `scram runtime -sh`
cd -
cd $EXCALIBUR_BASE
source $EXCALIBUR_BASE/scripts/ini_excalibur
cd -
$EXCALIBUR_BASE/artus $EXCALIBUR_BASE/cfg/artus/config.py.json