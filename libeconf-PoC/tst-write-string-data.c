#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include "libeconf.h"

/* Test case:
 * Reading, merging and writing string data
*/

int
main(int argc, char *argv[])
{
  econf_err error;  

  // Read test data
  econf_file *kf_fromfile;
  error = econf_readFile(&kf_fromfile,
			 argv[1],
			 "=", "#");
  if (error || kf_fromfile == NULL)
  {
    fprintf (stderr, "ERROR: couldn't read /usr/etc configuration file: %s\n", econf_errString(error));
    return 1;
  }

  // Rewrite file to disk
  econf_writeFile(kf_fromfile, "./corpus", "out.ini");

  // And reading it again
  econf_file *kf_compare;
  error = econf_readFile(&kf_compare,
			 "./corpus/out.ini", "=", "#");
  if (error || kf_compare == NULL) {
    fprintf (stderr, "ERROR: couldn't read written /usr/etc configuration file: %s\n", econf_errString(error));
    return 1;
  }
  remove("./corpus/out.ini");

  // Comparing the data
  const char *compare[] = {"test", "test2", "test3", "test4", "test5", "test6"};
  for (int i = 0; i < 6; i++) {
    char *val_fromfile, *val_compare;
    econf_getStringValue(kf_fromfile, "test", compare[i], &val_fromfile);
    econf_getStringValue(kf_compare, "test", compare[i], &val_compare);
    if (val_fromfile == NULL && val_compare == NULL)
      continue;
    if (val_fromfile == NULL || val_compare == NULL) {
      fprintf (stderr, "ERROR: saved values are different\n");
      return 1;
    }
    if(strcmp(val_fromfile, val_compare) != 0) {
      fprintf (stderr, "ERROR: saved values are different '%s' - '%s'\n",
	       val_fromfile, val_compare);
      return 1;
    }
    free(val_fromfile);
    free(val_compare);
  }

  // Merge two econf_files. One value is a NULL value.
  econf_file *kf_freshini;
  econf_newIniFile(&kf_freshini);
  econf_setStringValue(kf_freshini, "test", "test", NULL);  
  econf_file *kf_merged;
  econf_mergeFiles(&kf_merged, kf_freshini, kf_fromfile);

  econf_free(kf_freshini);
  econf_free(kf_merged);
  econf_free(kf_compare);
  econf_free(kf_fromfile);
  return 0;
}
