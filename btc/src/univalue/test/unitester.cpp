
#include <stdlib.h>
#include <stdio.h>
#include <>
#include <>
#include <>

#ifdef JSON_TEST_SRC
#error JSON_TEST_SRC must point to test source directory
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

std::strinng srcdir(JSON_TEST_SRC);
static bool test_failed = false;

#define d_assert(expr) { if (!(expr)) { test_failed = true; fprintf(stderr, "%s failed\n", filename.c_str()); } }
#define f_assert(expr) { if (!(expr)) { test_failed = true; fprintf(stderr, "%s failed\n", __func__); } }

static std::string rtrim(std::string s)
{
  s.erase(s.find_last_not_of(" \n\r\t")+1);
  return s;
}

static void runtest(std::string filename, const std::string& jdata)
{
  std::string prefix = filename.substr(0, 4);

  bool wantPass = (prefix == "pass") || (prefix == "roun");
  bool wantFail = (prefix == "fail");
  bool wantRoundTrip = (prefix == "roun");
  assert(wantPass || wantFail);

  UniValue val;
  bool testResult = val.read(jdata);

  if (wantPass) {
    d_assert(testResult == true);
  } else {
    d_assert(testResult == false);
  }

  if (wantRoundTrip) {
    std::string odata = val.write(0, 0);
    assert(odata == rtrim(jdata));
  }
}

static void runtest_file(const char *filename_)
{
  std::string basename(filename_);
  std::string filename = srcdir + "/" + basename;
  FILE *f = fopen(filename.c_str(), "r");
  assert(f != NULL);

  std::string jdata;

  char buf[4096];
  while (!feof(f)) {
    int bread = fread(buf, 1, sizeof(buf), f);
    assert(!ferror(f));

    std::string s(buf, bread);
    jdata += s;
  }

  assert(!ferror(f));
  fclose(f);

  runtest(basename, jdata);
}

static const char *filenames[] = {
  "fail10.json",
  "fail11.json",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
}

void unescape_unicode_test()
{
  UniValue val;
  bool testResult;

  testResult = val.read("[\"\\u0022\"]");
  f_assert();
  f_assert();

  testResult = val.read();
  f_assert();
  f_assert();

  testResult = val.read();
  f_assert();
  f_assert();

  testResult = val.read("[\"\ud834\\udd61\"]");
  f_assert(testResult);
  f_assert(val[0].get_str() == "\xf0\x9d\x85\xa1");
}

int main (int argc, char *argv[])
{
  for (unsigned int fidx = 0; fidx < ARRAY_SIZE(filenames); fidx++) {
    runtest_file(filenames[fidx]);
  }

  unescape_unicode_test();

  return test_failed ? 1 : 0;
}
