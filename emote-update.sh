#!/bin/bash
emotes=$(curl https://raw.githubusercontent.com/memelabs/chat-gui/master/assets/emotes.json | jq '.default | .[]')
list=""
for e in $emotes; do
	list+=$(echo $e | sed -e 's/^"//' -e 's/"$//'),
done

sed -ie "s/^EMOTES=.*/EMOTES=$(echo $list | rev | cut -c 2- | rev)/" .env.example
