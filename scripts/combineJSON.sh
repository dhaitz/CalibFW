# Combine JSON files for Rereco and PromptReco
# Usage: scripts/combineJSON.sh AB_rereco.json C_prompt.json

# Remove 2012AB runs from PromptReco json
filterJSON.py --min=197700 $2 --output tmp_384956746.json

# Add 2012AB 13JulRereco json
compareJSON.py --or $1 tmp_384956746.json ${2/PromptReco/CombinedReco}
echo "${2/PromptReco/CombinedReco} saved."

# Cleanup
rm tmp_384956746.json