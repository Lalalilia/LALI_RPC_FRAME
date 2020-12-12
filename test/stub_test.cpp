//
// Created by lalalilia on 11/30/20.
//

//
// Created by lalalilia on 11/16/20.
//

#include <unistd.h>
#include <iostream>
#include <jackson/Document.h>
#include <jackson/FileReadStream.h>
#include <zrpc/stub/StubGenerator.h>
#include <zrpc/stub/ClientStub.h>
#include <zrpc/stub/ServiceStub.h>

static void usage()
{
    fprintf(stderr,
            "usage: stub_generator <-c/s> [-o] [-i input]\n");
    exit(1);
}

static void writeToFile(StubGenerator& generator, bool outputToFile)
{
    FILE* output;
    if (!outputToFile) {
        output = stdout;
    }
    else {
        std::string outputFileName = generator.getStubClassName() + ".h";
        output = fopen(outputFileName.c_str(), "w");
        if (output == nullptr) {
            perror("error");
            exit(1);
        }
    }

    auto stubString = generator.getStub();
    fputs(stubString.c_str(), output);
}

static std::unique_ptr<StubGenerator>
makeGenerator(bool serverSide, json::Value& proto)
{
    if (serverSide)
        return std::make_unique<ServiceStubGenerator>(proto);
    else
        return std::make_unique<ClientStub>(proto);
}

// 用于建立存根，读取json文件
static void bulidStub(FILE* input, bool serverSide, bool outputToFile)
{
    // 解析json文件
    json::FileReadStream is(input);
    json::Document proto;
    auto err = proto.parseStream(is);
    if (err != json::PARSE_OK) {
        fprintf(stderr, "%s\n", json::parseErrorStr(err));
        exit(1);
    }
    auto generator = makeGenerator(serverSide, proto);
    writeToFile(*generator, outputToFile);

}

int main(int argc, char** argv)
{
    std::cout << "hello";
    bool serverSide = false;
    bool clientSide = false;
    bool outputToFile = false;
    const char* inputFileName = nullptr;
    int opt;
    //解析输入指令
    while ((opt = getopt(argc, argv, "csi:o")) != -1) {
        switch (opt) {
            case 'c':
                clientSide = true;
                break;
            case 's':
                serverSide = true;
                break;
            case 'o':
                outputToFile = true;
                break;
            case 'i':
                inputFileName = optarg;
                break;
            default:
                fprintf(stderr, "unknown flag %c\n", opt);
                usage();
        }
    }
    if (!serverSide && !clientSide) {
        serverSide = clientSide = true;
    }

    FILE* input = stdin;
    // 将json文件读取进入程序中
    if (inputFileName != nullptr) {
        input = fopen(inputFileName, "r");
        if (input == nullptr) {
            perror("error");
            exit(1);
        }
    }
    if (serverSide) {
        bulidStub(input, true, outputToFile);
        rewind(input);
    }
    if (clientSide) {
        bulidStub(input, false, outputToFile);
    }

}