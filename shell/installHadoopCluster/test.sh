#!/bin/bash

function replaceBlank2Comma(){
    context="$@"
    b=${context// /,}
    echo ${b}
}


function replaceComma2Blank(){
    context="$@"
    echo "$context"|sed "s/,/ /g"
}

echo "test blank to comma:"
blankStr='node1 node2 node3'
echo ${blankStr}
blankStr=$(replaceBlank2Comma ${blankStr})
echo ${blankStr}


echo "test comma to blank:"
commaStr='node1,node2,node3'
echo ${commaStr}
commaStr=$(replaceComma2Blank ${commaStr})
echo ${commaStr}

#IFS=,
content=""
for node in ${commaStr} ; do
    content=${content}${node}":2181,"
done
length=`expr ${#content} - 1`
echo ${length}
content=${content:0:$length}
echo ${content}
