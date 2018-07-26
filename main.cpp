#include <iostream>
#include <sstream>
#include <fstream>

#include <string.h>
#include <fcntl.h>

#include <unistd.h>

#define NARGS 15
#define ARGLEN 50

const char BAR = '|';


int nthSubstr(int, const std::string&, const char);

int get_pipe_counts(const std::string&);

void get_token(const std::string&, char**, int);

void recursion(const std::string&, int, int counter = 0, int dest = -1);

int create_file(std::string file = "/home/box/result.out");

void logging(const std::string&);

int main()
{
    std::string str="pwd | wc";

    getline(std::cin, str);

    logging(str);

    int nPipes = get_pipe_counts(str);

    recursion(str, nPipes);
    return 0;
}

int nthSubstr(int n, const std::string&s, const char p)
{
   std::string::size_type i =  s.find(p); //the first occurrence

   uint j;
   for(j=1;j!=std::string::npos&&j<n;++j)
       i=s.find(p, i+1); //the next occurrence
   if(j==n)
       return i;
   else
       return(-1);
}

int get_pipe_counts(const std::string &str)
{
    int c = 0;
    size_t pos = 0;

    while(std::string::npos != (pos = str.find('|', pos + 1)) )
        ++c;

    return c;
}

void get_token(const std::string &str, char**argv, int counter)
{

    int till=nthSubstr(counter+1, str, BAR);
    if(till==-1)
    {
        till=str.size();
    }

    int from=nthSubstr(counter, str, BAR)+1;
    if(from==-1)from=0;

    std::string working_expr = str.substr(from, till-from), word;

    std::stringstream ss(working_expr);

    int i=0;
    while(ss >> word)
    {
        strcpy(argv[i],word.c_str());
        ++i;
    }

    argv[i]=NULL;
}

void recursion(const std::string&cmds, int nPipes, int counter, int dest)
{
    int fds[] = {-1, -1};

    if(counter<nPipes)
    {
        pipe(fds);
        if(!fork())
            recursion(cmds,nPipes,++counter,fds[1]);
    }

    if(dest!=-1)
        dup2(dest, STDOUT_FILENO);
    else if(counter==0)
    {
        int f = create_file("result.out");
        dup2(f, STDOUT_FILENO);
        close(f);
    }

    if(fds[0] != -1)
        dup2(fds[0], STDIN_FILENO);
    close(fds[1]);

    char*argv[NARGS];
    for(int i = 0; i<NARGS; ++i)
        argv[i]=new char[ARGLEN];

    get_token(cmds, argv, nPipes - counter);

    execvp(argv[0], argv);
}

int create_file(std::string file)
{
    return creat(file.c_str(), O_WRONLY|S_IRWXU|S_IRWXO);//, O_CREAT | O_WRONLY, S_IRWXU);
}

void logging(const std::string&str)
{
    std::string filename("log.txt");
    std::fstream f(filename, f.out | f.app);

    if(f.is_open())
        f << str << "\n";
    else
        std::cout<<"Error open log file\n";
    f.close();
}
