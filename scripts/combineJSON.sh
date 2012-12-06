# Combine JSON files for Rereco and PromptReco
# Usage: scripts/combineJSON.sh ABC_rereco.jsons C_prompt.json

PROMPT=${@: -1}
OUTPUT=${PROMPT/PromptReco/CombinedReco}
TMP=tmp_384956746.json

# Remove 2012AB runs from PromptReco json
filterJSON.py --min=198934 --output="0_$TMP" $PROMPT

# Add 2012AB 13JulRereco json
i=0
for j in $*;
do
  if [[ "$j" != "$PROMPT" ]]
  then
    compareJSON.py --or "$j" "${i}_${TMP}" "$((i+1))_${TMP}"
    i=$((i+1))
  fi
done

mv "${i}_${TMP}" "$OUTPUT"
echo "$OUTPUT saved."
rm *$TMP