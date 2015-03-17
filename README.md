#svn-information
precondition:
	there is a svn work copy directory

usage:
	1. use generate_svninfo.sh to generate svn_info.c, which contains the svn informations
		$./generate_svninfo.sh
	2. add the svn_info.c to your makefile's source code files list
	3. compile and use the verinfo.c to get svn informations
