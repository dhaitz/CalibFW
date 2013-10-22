echo -e "Hidden\t" $(git ignored | grep -e \~ -e "\/\." | wc -l)
echo -e "root\t" $(git ignored | grep \.root | wc -l)
echo -e "pyc\t" $(git ignored | grep \.pyc | wc -l)
echo -e "png\t" $(git ignored | grep \.png | wc -l)
echo -e "pdf\t" $(git ignored | grep \.pdf | wc -l)
echo -e "object\t" $(git ignored | grep \.o | wc -l)
echo "Others:"
git ignored | grep -v \.root | grep -v \.pyc | grep -v \.png | grep -v \~ | grep -v \.o | grep -v \.pdf | grep -v "\/\."
