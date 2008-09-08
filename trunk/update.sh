#!/bin/bash

#把svn status 的結果送到temp檔
svn status > temp

#抓行數
total_line=`awk '{print NR}' temp|tail -n1`

#迴圈
for ((current_line=1;current_line<=$total_line;current_line=current_line+1))
do
	awk -v current=$current_line -v total=$total_line '{
		if ($1=="?" && current==NR && $2!="temp")
		{
			printf("%s\n",$0);
			printf("The target is %s\n",$2);
			printf("Do you want to add(y) or delete(n):");
			"read yn && echo $yn" | getline yn;
			if (yn == "y")
			{
				cmd = "svn add "$2;
				print(cmd);
				system(cmd);
			}
			else if (yn == "n")
			{
				cmd = "svn delete --force "$2;
				print(cmd);
				system(cmd);
			}
		}
		else if ($1=="!" && current==NR && $2!="temp")
		{
			printf("%s\n",$0);
			printf("Do you want to revert(y) or delete(n):");
			"read yn && echo $yn" | getline yn;
			if (yn == "y")
			{
				cmd = "svn revert "$2;
				print(cmd);
				system(cmd);
			}
			else if (yn == "n")
			{
				cmd = "svn delete "$2;
				print(cmd);
				system(cmd);
			}
		}
	}' temp
done
#刪除暫存檔temp
rm -rfv temp

