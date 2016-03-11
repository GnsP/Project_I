#!/bin/bash
echo "digraph G{"
for LINK in $(xmlstarlet el main.xml | grep -E -o '([^/]+)/([^/]+)$'|tr '-' '_'|sed 's/\//->/g' | tr ':' '_')
	do
		echo ${LINK}";"
	done
echo "}"

