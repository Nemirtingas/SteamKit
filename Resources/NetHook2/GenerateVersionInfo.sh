#! /bin/bash

buildDate="$(date +"%Y-%m-%d %H:%M:%S %z")"

if [ ! -z "$(type -P "git")" ]; then
  commitDate="$(git show -s --format="%ci" HEAD)"
  sha="$(git rev-parse --short HEAD)"
  git diff --quiet
  if [ $? -eq 0 ]; then
	dirty="false"
  else
	dirty="true"
  fi
else
  dirty="false"
fi

versionFile='NetHook2/version.cpp'

cat <<EOF >"${versionFile}"
#include "version.h"

const char *g_szBuildDate = "${buildDate}";
const char *g_szBuiltFromCommitSha = "${sha}";
const char *g_szBuiltFromCommitDate = "${commitDate}";
const bool g_bBuiltFromDirty = $dirty;
EOF
