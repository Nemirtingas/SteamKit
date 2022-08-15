#! /bin/bash

cd "$(dirname "$0")"


ThisDirectory="$(pwd)"
NativeDependenciesDirectory="${ThisDirectory}/native-dependencies"
NetHook2DependenciesTemporaryDirectory="${ThisDirectory}/tmp/nethook2-dependencies"
ZLibSourceZipUrl="https://zlib.net/zlib1212.zip"
ZLibSourceFile="${NetHook2DependenciesTemporaryDirectory}/zlib-1.2.12.zip"

ZLibSourceInnerFolderName="zlib-1.2.12"
ProtobufVersion="3.15.6"
ProtobufSourceZipUrl="https://github.com/protocolbuffers/protobuf/releases/download/v$ProtobufVersion/protobuf-cpp-$ProtobufVersion.zip"
ProtobufSourceFile="${NetHook2DependenciesTemporaryDirectory}/protobuf-${ProtobufVersion}.zip"
ProtobufSourceInnerFolderName="protobuf-${ProtobufVersion}"

mkdir -p "${NetHook2DependenciesTemporaryDirectory}" "${NativeDependenciesDirectory}"

ZLibFolderPath="${NativeDependenciesDirectory}/${ZLibSourceInnerFolderName}"
if [ ! -e "${ZLibFolderPath}" ]; then
	if [ ! -e "${ZLibSourceFile}" ]; then
		echo "Downloading ZLib source..."
		curl -Lk "${ZLibSourceZipUrl}" -o "${ZLibSourceFile}"
	fi

	echo "Extracting ZLib..."
	pushd "${NativeDependenciesDirectory}"
	unzip "${ZLibSourceFile}"
	popd
fi

ProtobufFolderPath="${NativeDependenciesDirectory}/${ProtobufSourceInnerFolderName}"
if [ ! -e "${ProtobufFolderPath}" ]; then
	if [ ! -e "${ProtobufSourceFile}" ]; then
		echo "Downloading Google Protobuf source..."
		curl -Lk "${ProtobufSourceZipUrl}" -o "${ProtobufSourceFile}"
	fi

	echo "Extracting Protobuf..."
	pushd "${NativeDependenciesDirectory}"
	unzip "${ProtobufSourceFile}"
	popd
fi
