#!/bin/bash

#把svn status 的結果送到temp檔
svn status > temp

#抓行數
total_line=`awk '{print NR}' temp|tail -n1`

#迴圈
for ((current_line=1;current_line<=$total_line;current_line=current_line+1))
do
	awk -v current=$current_line -v total=$total_line '{
		if ($1=="?" && current==NR)
		{
			printf("%s\n",$0);
			printf("Do you want to add(y) or delete(n):");
			"read yn && echo $yn" | getline yn;
			if (yn == "y")
				system("svn add $2");
			else
				system("svn delete $2");
		}
		else if ($1=="!" && current==NR)
		{
			printf("%s\n",$0);
			printf("Do you want to revert(y) or delete(n):");
			"read yn && echo $yn" | getline yn;
			if (yn == "y")
				system("svn revert $2");
			else
				system("svn delete --forece $2");
		}
	}' temp
done
#awk '{
#	if ($1=="?") 
#	{
#		printf("%s\n",$0);
#		printf("Do you want to add(y) or delete(n):");
#		"read yn && echo $yn" | getline yn;
#	}
#	else
#	{
#		printf("%d\n",NR)
#		"read yn && echo $yn" | getline pp;
#		printf("%s\n",pp);
#		if (pp == "y")
#			printf("good\n");
#	}
#}' test
