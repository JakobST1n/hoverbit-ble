#!/bin/bash

if output=$(git status --porcelain) && [ -z "$output" ]; then
    echo "Git working directory is clean."
else
    echo "Git working directory is not clean..."
    exit 1
fi

tput setaf 4
echo "> Bump version number"
tput sgr0

if [ $# -gt 0 ]; then
    npm version "$1" || exit 1;
else
    npm version patch || exit 1;
fi

VERSION=$(cat package.json \
  | grep version \
  | head -1 \
  | awk -F: '{ print $2 }' \
  | sed 's/[",]//g' \
  | tr -d '[[:space:]]')
echo "Deploying for tag: $VERSION."

tput setaf 4
echo "> Build app"
tput sgr0

rm -r dist/
npm install
npm run build



tput setaf 4
echo "> Copy to gh-pages branch and commit"
tput sgr0

cp .gitignore dist/.gitignore
git checkout gh-pages || git checkout --orphan gh-pages
git rm -rf .

cp dist/.gitignore .gitignore
cp -r dist/* .

sed -i".bak" "s/{{ VERSION }}/$VERSION/g" index.html
rm index.html.bak

git add .
git commit -m ":rocket: Deploy app v$VERSION"


tput setaf 4
echo "> Return to controller branch and tag last commit"
tput sgr0

git checkout controller

#git push --follow-tags origin controller
#git push origin gh-pages
