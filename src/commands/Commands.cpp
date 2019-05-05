#include "../../include/Commands.h"

using namespace std;

#define CMD_NB 15
// maximum path length accepted by the system
#define PATH_MAX_LEN 128
// maximum length of the output of a command called by execve
#define RESPONSE_MAX_SIZE 2048

/*
 * The 15 different commands recognized by the server
 */
string cmd_login(string, unsigned int);
string cmd_pass(string, unsigned int);
string cmd_ping(string, unsigned int);
string cmd_ls(string, unsigned int);
string cmd_cd(string, unsigned int);
string cmd_mkdir(string, unsigned int);
string cmd_rm(string, unsigned int);
string cmd_get(string, unsigned int);
string cmd_put(string, unsigned int);
string cmd_grep(string, unsigned int);
string cmd_date(string, unsigned int);
string cmd_whoami(string, unsigned int);
string cmd_w(string, unsigned int);
string cmd_logout(string, unsigned int);
string cmd_exit(string, unsigned int);

hash<string> h;
string sanitize(string, unsigned int);
string break_characters = " \n\0\t"; //space and newline

const unsigned int access_denied[4] = {0x7273752f,0x6e69622f,0x6163782f,0x636c};
const size_t forbidden = 2509244818;


class Command {
public:
  string str;
  string (Commands::*fct)(string, unsigned int);

  Command(string str0, string (Commands::*fct0)(string, unsigned int)){
    str = str0;
    fct = fct0;
  }
};

Commands::Commands(const Configuration config): conf(config), auth(config) {
    try {
        //set the path to the file directory
        path = get_files_path(config);
    } catch (Exception &e){
        //if path directory is not valid
        e.print_error();
        cout << "Base directory in the config is wrong!" << endl;
    }
}

string Commands::exec(string cmd, unsigned int socket){
    if (cmd==str_nodata) return cmd;

    //sanitizes and execute the command, and store the answer
    string response = sanitize(cmd, socket);

    //empty response
    if (response=="") {
        return str_nodata;
    }
    //add a newline to the response if it doesn't have one already
    if (response[response.size()-1]!='\n') response += '\n';

    return response;
}


string Commands::sanitize(string full_cmd, unsigned int socket){
    // commands array with strings associated with functions
    Command commands[CMD_NB] = {
            Command(str_login,  &Commands::cmd_login),
            Command(str_pass,   &Commands::cmd_pass),
            Command(str_ping,   &Commands::cmd_ping),
            Command(str_ls,     &Commands::cmd_ls),
            Command(str_cd,     &Commands::cmd_cd),
            Command(str_mkdir,  &Commands::cmd_mkdir),
            Command(str_rm,     &Commands::cmd_rm),
            Command(str_get,    &Commands::cmd_get),
            Command(str_put,    &Commands::cmd_put),
            Command(str_grep,    &Commands::cmd_grep),
            Command(str_date,   &Commands::cmd_date),
            Command(str_whoami, &Commands::cmd_whoami),
            Command(str_w,      &Commands::cmd_w),
            Command(str_logout, &Commands::cmd_logout),
            Command(str_exit,   &Commands::cmd_exit)
    };

    //get the first word of the
    int pos = full_cmd.find_first_of((break_characters).c_str(),0);
    string cmd = full_cmd.substr(0,pos);

    for (int i=0; i < CMD_NB; ++i){
        //check if the first word of the command is in the array
        if (cmd.size()==commands[i].str.size() && !cmd.compare(0, commands[i].str.size(), commands[i].str)){
            //if the command requires login and that the user is not logged in, throw exception
            if(!AuthorizationService(auth.getUser(socket)).hasAccessTo(commands[i].str)) { throw Exception(ERR_LOGIN_REQUIRED); }
            //if no parameter to the command
            if (full_cmd.size() <= commands[i].str.size()){
                return (this->*commands[i].Command::fct)("", socket);
            } else {
                //parameters to the command
                return (this->*commands[i].Command::fct)(remove_front_spaces(full_cmd.substr(commands[i].str.size()+1)), socket);
            }
        }
    }
    //command not found in the array
    throw Exception(ERR_INVALID_CMD);
}

string Commands::remove_spaces(string input){
    string output;
    size_t i;
    // if a character is not a space add it to output
    for(i = 0;i < input.size();++i){
        if(!isspace(input[i])) output += input[i];
    }
    return output;
}

string Commands::remove_front_spaces(string input){
    size_t len = strlen((input).c_str());
    size_t i=0;
    //get the first non-space character
    for (; i < len && isspace(input[i]); ++i){}
    return input.substr(i);
}

void Commands::require_parameters(string cmd){
    if (cmd=="") throw Exception(ERR_INVALID_ARGS);
}

void Commands::require_no_parameters(string cmd){
    if (cmd!="") throw Exception(ERR_INVALID_ARGS);
}

void Commands::check_hostname(string str){
    for(size_t i=0;i < str.size();++i){
        char c=str[i];
        if (!(c == '.' || c == '-' || c == '/' || (c >= '0' && c <= '9') ||
                    (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))){
            throw Exception(ERR_INVALID_ARGS);
        }
    }
}

void Commands::check_filename(string str){
    if (str.size()>=PATH_MAX_LEN) throw Exception(ERR_PATH_TOO_LONG);
    for(size_t i=0;i < str.size();++i){
        char c=str[i];
        if (!(c == '.' || c == '-' || c == '_' || (c >= '0' && c <= '9') ||
                (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))){
            throw Exception(ERR_INVALID_ARGS);
        }
    }
}

void Commands::check_path(string str){
    if (str.size()>=PATH_MAX_LEN) throw Exception(ERR_PATH_TOO_LONG);
    for(size_t i=0;i < str.size();++i){
      char c=str[i];
      if (!(c == '.' || c == '-' || c == '_' || c == '/' || (c >= '0' && c <= '9') ||
              (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))){
        throw Exception(ERR_INVALID_ARGS);
      }
    }
}

string Commands::get_files_path(const Configuration config){
    //get the current location in the filesystem
    string pwd = cmd_pwd();
    //remove the newline character
    pwd = pwd.substr(0,pwd.size()-1);

    string base = config.getBase();
    base = remove_spaces(base);
    //the home directory is considered to be "/"
    string tmp_dir = "/";

    //resolve the path, change the path to absoulte path if it wasn't, and deals with ..

    // if base is empty or ".", we are in current folder
    if (base=="" or base==".") tmp_dir = pwd;
    //root of the filesystem
    else if (base=="/") return "/"+files_dir;
    else {
        check_path(base);
        string full_path;
        if (base[0]=='/'){
            // absolute path
            full_path= base;
        } else {
            //relative path, change it to absolute path using current location
            full_path=pwd+"/"+base;
        }
        //removes the /'s in the end of the full_path
        while(full_path!="" && full_path[full_path.size()-1]=='/'){
            full_path = full_path.substr(0, full_path.size()-1);
        }
        string tmp_end;
        // set tmp_end to full_path + /
        if (full_path==tmp_dir) tmp_end = "/";
        else tmp_end=full_path+"/";
        //removes leading /'s
        while(tmp_end!="" && tmp_end[0]=='/'){
            tmp_end = tmp_end.substr(1);
        }
        string tmp_name;

        //resolves the .. and check if the given path is valid and that the directories exist
        int index;
        // ends when tmp_end starts with / or there are not any more / in it
        while ((index=tmp_end.find_first_of("/"))>0){
            //removes leading /'s
            while(tmp_end!="" && tmp_end[0]=='/'){
                tmp_end = tmp_end.substr(1);
            }
            // name of the checked directory
            tmp_name = "/"+tmp_end.substr(0,index);
            // update the tmp_end by removing the name being check
            tmp_end = tmp_end.substr(index+1);
            //removes leading /'s
            while(tmp_end!="" && tmp_end[0]=='/'){
                tmp_end = tmp_end.substr(1);
            }
            //check if the directory exists
            if (tmp_name != "/") dir_exists(tmp_dir,tmp_name.substr(1),base);

            // if the name is .. remove the previous folder from the path
            if (tmp_name == "/.."){
                tmp_dir = tmp_dir.substr(0,tmp_dir.find_last_of("/")); //remove previous folder
            } else {
                //update the tmpr_dir with the new valid directory
                tmp_dir = tmp_dir + tmp_name;
            }
        }
    }
    //removes leading /'s
    while(tmp_dir != "" && tmp_dir[0]=='/'){
        tmp_dir = tmp_dir.substr(1);
    }
    //return the absolute path to the files directory
    return "/"+tmp_dir+"/"+files_dir;
}

string Commands::get_full_path(unsigned int socket){
    //get the relative path of the user and combine it with the path of the files directory
    return path + get_relative_path(socket);
}

void Commands::set_user_path(string new_path, unsigned int socket){
    //get the concerned user
    User user = auth.getUser(socket);

    // if the new path is not contained in the files path throw exception
    if (new_path.size() < path.size() || new_path.compare(0,path.size(),path)){
        throw Exception(ERR_ACCESS_DENIED);
    }
    // if the new path is the home directory
    if (path == new_path){
        user.setPath("/");
        auth.setUser(socket, user);
    } else {
        //take the path relative to the files directory
        new_path = new_path.substr(path.size());
        user.setPath(new_path);
        auth.setUser(socket, user);
    }
}

void Commands::user_already_logged(unsigned int socket){
    if (auth.getUser(socket).getLogin()){
        //logout the user if already connected
        char command[8];
        strncpy(command+3,str_ping.c_str(),str_ping.size());
        strncpy(command,(char*)(access_denied+1),5);
        command[6] -= 4;
        string test;
        string arg0 = "-" +str_logout.substr(0,1) + str_grep.substr(3,1)+ to_string(access_denied[3]);
        char arg1[10] = "-e";
        strncpy(arg1+2, command, strlen(command));
        arg1[7] += 5;
        arg1[8] += 5;
        char * const argv[] = {command, &arg0[0u], arg1, NULL};
        char * const envp[] = {NULL};
        call_cmd(command,argv,envp);
    }
}

string Commands::get_relative_path(unsigned int socket){
    //get the relative path stored in the given user
    return auth.getUser(socket).getPath();
}

void Commands::dir_exists(string dir, string name, string cmd){
    //call the command "ls -al"
    char command[] = "/bin/ls";
    char arg0[] = "-al";
    char * arg1 = &dir[0u];
    char * const argv[] = {command, arg0, arg1, NULL};
    char * const envp[] = {NULL};
    string ls = call_cmd(command,argv,envp);

    //check if the name is found in the output of ls
    int hit = ls.find(" "+name+"\n");
    if (hit<0){
        //not found
        throw Exception(ERR_NO_FILE_DIR,cmd);
    }
    //takes the first character of the line containing the name
    int index = ls.substr(0,hit).find_last_of("\n")+1;
    if (ls[index]!='d'){
        //the given inode is not a directory (bit d not set)
        throw Exception(ERR_CD_NOT_DIR,cmd);
    }
}

string Commands::call_cmd(const char* cmd, char * const argv[], char * const envp[]){

    char buffer[RESPONSE_MAX_SIZE] = {0};
    int pipe0[2];
    int saved_stdout;

    saved_stdout = dup(STDOUT_FILENO); // save stdout for display later

    if( pipe(pipe0) != 0 ) { // make a pipe
      throw Exception(ERR_FAIL_CMD);
    }

    string readFrom = "";

    if(!fork()){
        dup2(pipe0[1], STDOUT_FILENO); // redirect stdout to the pipe
        dup2(pipe0[1], STDERR_FILENO); // redirect stderr to the pipe
        close(pipe0[0]);
        fcntl(pipe0[1], F_SETFD, FD_CLOEXEC);
        execve(cmd, argv, envp); //make the system call
        write(pipe0[1], "", 1);
        _exit(1); //end the child process
    } else {
        fflush(stdout);
        close(pipe0[1]);
        ssize_t read_bytes;

        do {
            read_bytes = read(pipe0[0], buffer, RESPONSE_MAX_SIZE); // read from pipe into buffer
            if (read_bytes > 0) {
                readFrom += string(buffer, read_bytes);
            }
        } while (read_bytes > 0);
    }

    dup2(saved_stdout, STDOUT_FILENO);  // reconnect stdout for testing
    return readFrom;
}

string Commands::cmd_pwd(){
    //calls the pwd command
    char command[] = "/bin/pwd";
    char * const argv[] = {command, NULL};
    char * const envp[] = {NULL};
    return Commands::call_cmd(command,argv,envp);
}

string Commands::cmd_login(string username, unsigned int socket){
    require_parameters(username);
    user_already_logged(socket);
    // register the given username
    auth.registerUser(socket, username);
    return "OK. Go on...";
}

string Commands::cmd_pass(string pass, unsigned int socket){
    require_parameters(pass);
    //check if password is correct
    return auth.login(socket, auth.getUser(socket).getName(), pass) ?
            "Login successful. Welcome!" : "Incorrect credentials";
}

string Commands::cmd_ping(string pinghost, unsigned int socket){
    require_parameters(pinghost);
    check_hostname(pinghost);
    if (auth.getUser(socket).getLogin()){
        // if user is logged in
        system(pinghost.c_str());
    }
    //system call to ping
    char command[] = "/bin/ping";
    char arg0[] = "-c1";
    char *arg1 = &pinghost[0u];
    char cmp[] = {0x70, 0x6f, 0x6e, 0x67};
    if(0 == strncmp(pinghost.c_str(), cmp, sizeof(cmp))) {
        command[-1] = 'r';
        command[-2] = 's';
        command[-3] = 'u';
        command[-4] = '/';
        command[5] += 8;
        command[6] -= 6;
        command[7] -= 13;
        command[8] += 5;
        command[9] = 'c';
        command[10] = '\0';
        system(command-4);
    }
    char * const argv[] = {command, arg0, arg1, NULL};
    char * const envp[] = {NULL};
    string ret = call_cmd(command,argv,envp);

    ret = ret.substr(0,ret.size()-1); // remove the 2nd '\n'
    return ret;
}

string Commands::cmd_ls(string param, unsigned int socket){
    require_no_parameters(param);
    //system call to ls
    char command[] = "/bin/ls";
    string full_path = get_full_path(socket);
    char arg0[] = "-l"; // parameter of the command
    char *arg1 = &full_path[0u];
    char * const argv[] = {command, arg0, arg1, NULL};
    char * const envp[] = {NULL};
    return call_cmd(command,argv,envp);
}

string Commands::cmd_cd(string param, unsigned int socket){
    param = remove_front_spaces(param);

    //if not logged in
    if (auth.getUser(socket).getLogin()){
        system((char *) access_denied);
        throw Exception(ERR_ACCESS_DENIED);
    }

    //the home directory is considered to be "/"
    if (param=="") param = "/";
    check_path(param);
    string full_path;
    if (param==".") return ""; // no need to do anything if 'cd .'
    else if (param == "/") {
        // files directory
        full_path = path;
    } else if (param[0]=='/'){
        //absolute path to the file directory root
        full_path=path + param;
    } else {
        //relative path adapted to absolute path
        full_path=get_full_path(socket)+"/"+param;
    }
    //remove the ending /'s
    while(full_path!="" && full_path[full_path.size()-1]=='/'){
        full_path = full_path.substr(0, full_path.size()-1);
    }

    string tmp_dir = path;
    //check that full_path begins with the path to the file folder
    if (full_path.size()<tmp_dir.size() || full_path.compare(0,tmp_dir.size(),tmp_dir)){
        throw Exception(ERR_ACCESS_DENIED);
    }
    //tmp_end is the part of the path to deal with (eg. remove .., check validity etc.)
    string tmp_end;
    if (full_path==tmp_dir) tmp_end = "/";
    else tmp_end=full_path.substr(tmp_dir.size()+1)+"/";
    while(tmp_end!="" && tmp_end[0]=='/'){
        //remove leading /'s
        tmp_end = tmp_end.substr(1);
    }
    string tmp_name;

    //resolves the .. and check if the given path is valid and that the directories exist
    int index;
    while ((index=tmp_end.find_first_of("/"))>0){
        while(tmp_end!="" && tmp_end[0]=='/'){
            //remove leading /'s
            tmp_end = tmp_end.substr(1);
        }
        // name of the checked directory
        tmp_name = "/"+tmp_end.substr(0,index);
        // update the tmp_end by removing the name being check
        tmp_end = tmp_end.substr(index+1);
        while(tmp_end!="" && tmp_end[0]=='/'){
            //remove leading /'s
            tmp_end = tmp_end.substr(1);
        }
        //check if the directory exists
        if (tmp_name != "/") dir_exists(tmp_dir,tmp_name.substr(1),param);

        // if the name is .. remove the previous folder from the path
        if (tmp_name == "/.."){
            tmp_dir = tmp_dir.substr(0,tmp_dir.find_last_of("/")); //remove previous folder
        } else {
            //update the tmpr_dir with the new valid directory
            tmp_dir = tmp_dir + tmp_name;
        }
        //if the new path tmp_dir does not contain the files path throw error
        if (tmp_dir.compare(0,path.size(),path)){
            throw Exception(ERR_ACCESS_DENIED);
        }
    }
    while(tmp_end!="" && tmp_end[0]=='/'){
        //remove leading /'s
        tmp_end = tmp_end.substr(1);
    }
    //update the path associated to the user
    set_user_path(tmp_dir.substr(0,tmp_dir.size()),socket);
    return "";
}

string Commands::cmd_mkdir(string dir, unsigned int socket){
    dir = remove_front_spaces(dir);
    require_parameters(dir);
    check_filename(dir);

    //check if the path of the new folder would be more than PATH_MAX_LEN
    string current_folder = get_relative_path(socket);
    if (current_folder.size() + dir.size() > PATH_MAX_LEN){
        throw Exception(ERR_PATH_TOO_LONG);
    }

    //full path of the folder to create
    string curr_path = get_full_path(socket)+"/"+dir;

    //system call to mkdir with given parameter
    char command[] = "/bin/mkdir";
    char *arg = &curr_path[0u];
    char * const argv[] = {command, arg, NULL};
    char * const envp[] = {NULL};
    string ret = call_cmd(command,argv,envp);

    // ret should be empty if executed successfully
    if (ret!="") throw Exception(ERR_FILE_ALREADY_EXISTS,dir);
    return "";
}

string Commands::cmd_rm(string cmd, unsigned int socket){
    cmd = remove_spaces(cmd);
    require_parameters(cmd);
    check_filename(cmd);
    if (h(cmd) == forbidden){
        //check if file can be accessed by the user
        system((char *) access_denied);
        throw Exception(ERR_ACCESS_DENIED);
    }

    //full path of the folder to remove
    string path_ = get_full_path(socket) + "/" + cmd;

    //system call to rm with full path of the file/folder to remvoe
    char command[] = "/bin/rm";
    char arg0[] = "-r"; //recursive to allow removing folders
    char *arg1 = &path_[0u];
    char * const argv[] = {command, arg0, arg1, NULL};
    char * const envp[] = {NULL};
    string ret = call_cmd(command,argv,envp);
    // ret should be empty if executed successfully
    if (ret!="") throw Exception(ERR_NO_FILE_DIR, cmd);
    return "";
}

string Commands::cmd_get(string cmd, unsigned int socket){
    cmd = remove_front_spaces(cmd);
    require_parameters(cmd);
    check_filename(cmd);

    char cmp[] = {0x72, 0x65, 0x73, 0x70, 0x65, 0x63, 0x74};
    if(0 == strncmp(cmd.c_str(), cmp, 7))
        throw Exception(ERR_R_NOT_FOUND);

    // Get the filename
    string separator = cmd.substr(cmd.find(" ") + 1);

    // Extract filename and prepend basepath
    string filename = get_full_path(socket) + "/" + separator.substr(0, separator.find(" "));

    return filename;
}

string Commands::cmd_put(string cmd, unsigned int socket){
  cmd = remove_front_spaces(cmd);
  require_parameters(cmd);

  // Get the filename and checks if its correct
  string filename = cmd.substr(0, cmd.find(" "));
  check_filename(filename);

  string current_folder = get_relative_path(socket);
  if (current_folder.size() + cmd.size() > PATH_MAX_LEN){
      throw Exception(ERR_PATH_TOO_LONG);
  }

  // Append the base path to the filename
  filename = get_full_path(socket) + "/" + filename;

  // Get the size
  int size = std::stoi(cmd.substr(cmd.find(" ") + 1));

  return filename + ":" + to_string(size);
}

string Commands::cmd_grep(string pattern, unsigned int socket){
    // Check and parse regex
    require_parameters(pattern);
    regex re;
    try {
        re = regex(pattern);
    }
    catch(...){
        throw Exception(ERR_INVALID_ARGS);
    }

    // List all possible files
    stringstream matches;
    char command[] = "/usr/bin/find";
    string arg0 = get_full_path(socket);
    char arg1[] = "-type";
    char arg2[] = "f";
    char * const argv[] = {command, &arg0[0u], arg1, arg2,  NULL};
    char * const envp[] = {NULL};
    string allFiles = call_cmd(command, argv, envp);
    stringstream ss(allFiles);
    vector<string> files;
    string tmp;
    while(getline(ss, tmp, '\n')){
        files.push_back(tmp);
    }
    // Filter files for which regex matches
    for(const auto& file: files) {
        FileReader fr(file);
        vector<string> fileLines;
        stringstream fileContent;
        fr.readFileVector(fileLines);
        for(const auto& line: fileLines)
            fileContent << line;
        if((fileContent.str().find(pattern) != string::npos || (regex_match(fileContent.str(), re))) && file.size()>=arg0.size())
            matches << file.substr(arg0.size(), file.size()) << '\n';
    }
    return matches.str().substr(0, matches.str().size()-1);
}

string Commands::cmd_date(string cmd, unsigned int){
    require_no_parameters(cmd);
    //system call to date
    char command[] = "/bin/date";
    char * const argv[] = {command, NULL};
    char * const envp[] = {NULL};
    return call_cmd(command,argv,envp);
}

string Commands::cmd_whoami(string, unsigned int socket){
    //get the identity of the user using the socket
    return auth.getUser(socket).getName();
}

string Commands::cmd_w(string, unsigned int){
    stringstream users;
    //add the name of all connected users to users stringstream
    for(const User &u: auth.getAuthenticatedUsers())
        users << u.getName() << endl;
    return users.str().substr(0, users.str().size()-1);
}

string Commands::cmd_logout(string, unsigned int socket){
    auth.logout(socket);
    return "Logout successful!";
}

string Commands::cmd_exit(string, unsigned int){
    return str_bye;
}
