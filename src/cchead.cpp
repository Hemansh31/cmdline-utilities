#include <bits/stdc++.h>
#include <getopt.h>
#include <cstring>

using namespace std;

#define BYTE_TERMINATED     0
#define LINE_TERMINATED     1
#define ZERO_TERMINATED     2

static const struct option long_options[] = {
    {"bytes", required_argument, NULL, 'c'},
    {"lines", required_argument, NULL, 'n'},
    {"quiet", no_argument, NULL, 'q'},
    {"zero-terminated", no_argument, NULL, 'z'},
    {"help", no_argument, NULL, 'h'},
    {"verbose", no_argument, NULL, 'v'},
    {0, 0, 0, 0}
};

static const char* short_options = "c:n:qvz";

static void print_help()
{
    cout << "Usage: cchead [OPTION]... [FILE]...\n";
    cout << "Print the first 10 lines of each FILE to standard output.\n";
    cout << "With more than one FILE, precede each with a header giving the file name.\n\n";
    cout << "With no FILE, or when FILE is -, read standard input.\n\n";
    cout << "Mandatory arguments to long options are mandatory for short options too.\n";
    cout << "  -c,  --bytes=[-]NUM      print the first NUM bytes of each file;\n";
    cout << "                             with leading '-' print all but last NUM bytes\n";
    cout << "  -n,  --lines=[-]NUM      print the first NUM lines instead of first 10;\n";
    cout << "                             with leading '-' print all but last NUM lines\n";
    cout << "  -q,  --quiet             never print headers giving file names;\n";
    cout << "  -v,  --verbose           always print headers giving file names;\n";
    cout << "  -z,  --zero-terminated   line delimiter is NUL, not newline;\n";
    cout << "       --help              display this help and exit;\n";
    cout << '\n';
    cout << "NUM may have a multiplier suffix:'\n";
    cout << "b 512, kB 1000, K 1024, MB 1000*1000, M 1024*1024,\n";
    cout << "GB 1000*1000*1000, G 1024*1024*1024\n";
    return;
}

struct Config
{
    bool verbose;
    uint8_t boundary_type_;
    uint64_t boundary_count_;
};

static Config kRuntimeConfig = { true, LINE_TERMINATED, 10 };

const std::map<string, uint64_t> kValidSuffixValueMap =
{
    {"b", 512},
    {"kB", 1000},
    {"K", 1024},
    {"MB", 1000*1000},
    {"M", 1024*1024},
    {"GB", 1000*1000*1000},
    {"G", 1024*1024*1024}
};

static inline void PrintHeader(const char* header)
{
    cout << "====>" << header << "<====";
}

static bool IsValidSize(const char* str)
{
    string num = "";
    string suffix = "";
    int idx = 0;
    while(str[idx] != 0)
    {
        if (str[idx] >= '0' && str[idx] <= '9')
        {
            if (suffix.empty())  num += str[idx];
            else return false;
        }
        else
        {
            if (num.empty()) return false;
            else suffix += str[idx];
        }
        idx++;
    }
    if (!suffix.empty() && kValidSuffixValueMap.find(suffix) == kValidSuffixValueMap.end())
    {
        return false;
    }
    return true;
}

static uint64_t ConvertToSize(const char* str)
{
    if (!IsValidSize(str))
    {
        throw "Invalid Size Identifier";
    }
    string num = "";
    string suffix = "";
    int idx = 0;
    while(str[idx] != 0)
    {
        if (str[idx] >= '0' && str[idx] <= '9')
        {
            num += str[idx];
        }
        else
        {
            suffix += str[idx];
        }
        idx++;
    }
    uint64_t answer = stoull(num);
    answer *= (suffix.empty() ? 1ll : kValidSuffixValueMap.at(suffix));
    return answer;
}

static std::unique_ptr<ifstream> GetInputStream(const char* file_name)
{
    auto input_stream = std::unique_ptr<ifstream>(new ifstream(file_name));
    if (input_stream->fail())
    {
        string exception_message = "Error opening file: ";
        exception_message.append(strerror(errno));
        throw exception_message;
    }
    return input_stream;
}

int main(int argc, char * argv[])
{
    int option;
    int option_index = 0;
    while((option = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1)
    {
        switch (option)
        {
            case 'c':
                kRuntimeConfig.boundary_type_ = BYTE_TERMINATED;
                if (!IsValidSize(optarg))
                {
                    cerr << "Invalid size identifier " << optarg << '\n';
                    cerr << "Try cchead --help for more information\n";
                    return 1;
                }
                kRuntimeConfig.boundary_count_ = ConvertToSize(optarg);
                break;
            case 'n':
                if (kRuntimeConfig.boundary_type_ == BYTE_TERMINATED)
                {
                    kRuntimeConfig.boundary_type_ = LINE_TERMINATED;
                }
                if (!IsValidSize(optarg))
                {
                    cerr << "Invalid size identifier " << optarg << '\n';
                    cerr << "Try cchead --help for more information\n";
                    return 1;
                }
                kRuntimeConfig.boundary_count_ = ConvertToSize(optarg);
                break;
            case 'q':
                kRuntimeConfig.verbose = false;
                break;
            case 'z':
                kRuntimeConfig.boundary_type_ = ZERO_TERMINATED;
                break;
            case 'v':
                kRuntimeConfig.verbose = true;
                break;
            case '?':
                cerr << "Try cchead --help for more information\n";
                return 1;
            case 'h':          
            default:
                print_help();
                return 1;
        }
    }
    std::unique_ptr<ifstream> input_stream;
    bool is_std_in = false;
    int file_count = argc - optind;
    while(true)
    {
        try
        {
            if ((optind < argc) && (strcmp(argv[optind], "-") != 0))
            {
                input_stream = GetInputStream(argv[optind]);
                if (kRuntimeConfig.verbose && (file_count > 1))
                {
                    PrintHeader(argv[optind]);
                    cout << '\n';
                }
            }
            else
            {
                is_std_in = true;
                if (kRuntimeConfig.verbose && (file_count > 1))
                {
                    PrintHeader("Standard Input");
                    cout << '\n';
                }
            }
            char c;
            uint64_t cnt = kRuntimeConfig.boundary_count_;
            while(!(is_std_in ? cin.get(c).eof() : input_stream->get(c).eof()) && (cnt > 0))
            {
                cout.put(c);
                switch (kRuntimeConfig.boundary_type_)
                {
                    case BYTE_TERMINATED:
                        cnt--;
                        break;
                    case LINE_TERMINATED:
                        if (c == '\n') cnt--;
                        break;
                    case ZERO_TERMINATED:
                        if (c == 0) cnt--;
                        break;
                }
            }
        }
        catch(const std::exception& e)
        {
            cerr << e.what() << '\n';
        }
        optind++;
        if (file_count > 1 && optind < argc) cout << '\n';
        if (optind >= argc) break;
    }
    return 0;
}