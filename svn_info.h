#ifndef _SVN_INFO_H_
#define _SVN_INFO_H_

#define MAX_AUTHER_NAME_LEN 		56
#define MAX_SVNURL_INFO 			256
#define MAX_WORK_DIR_LEN			256
#define WORK_REV_LEN				24
#define MAX_LAST_REV_LEN			24
#define MAX_COMPILE_DATE_INFO_LEN	56

#define VERINFO_SECTION_NAME ".xc.info_ver"

struct svn_informations
{
	char author[MAX_AUTHER_NAME_LEN];
	char svnurl_info[MAX_SVNURL_INFO];
	char work_dir[MAX_WORK_DIR_LEN];
	char work_rev[WORK_REV_LEN];
	char last_src_rev[MAX_LAST_REV_LEN];
	char last_date_info[MAX_COMPILE_DATE_INFO_LEN];
};

#define DEFINE_SVN_INFO(auth_info, url, work_copy, rev, last_rev, date) \
struct svn_informations _this_infor __attribute__((section (VERINFO_SECTION_NAME)))=\
{\
	.author=auth_info,\
	.svnurl_info=url,\
	.work_dir=work_copy,\
	.work_rev=rev,\
	.last_src_rev=last_rev,\
	.last_date_info=date,\
}

#endif//_SVN_INFO_H_
