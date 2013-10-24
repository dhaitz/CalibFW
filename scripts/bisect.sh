make -j4
#condition for your problem:
EXCDE=$(python cfg/artus/mc_madgraphSummer12.py --fast 1 | grep secondjet_pt | grep nan | wc -l)

echo $EXCDE
exit $EXCDE
