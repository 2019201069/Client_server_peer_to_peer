
#include "cliutil.h"

int sockfd;
struct sockaddr_in ser_addr;
//down:filesize:(uname:ip:port:path)

// down:38:aaa:127.0.0.1:12347:/home/apurva/Downloads/SocketProg-20180423T161710Z-001/SocketProg/anj.txt:;:kkk.txt;

//   ./cli 127.0.0.1:12345 
//  g++ trackercli.cpp -lpthread -lcrypto -o cli

// runs in main

void downloadactualfile(string downldmsg){    // create thread for each client 

    cout<<"first line toh print ho ja"<<endl;
    int portno;
    char buffer[BUFFSIZE];
    struct sockaddr_in cli_addr;
    int n;
     
    string ipfordownload[MAXCLIENTS];
    string fromclient[MAXCLIENTS];
    int portfordownload[MAXCLIENTS];
    string pathfordownload[MAXCLIENTS];

    // remove down and total filesize
    strtok((char *)downldmsg.c_str(), COL);  // remove down
    int filesize = atoi(strtok(NULL, COL));
    //string end = strtok(NULL, COL);
    cout<<"file size is :"<<filesize<<endl;
    int i = 0;

    string params[BUFFSIZE];
    params[0] = strtok(NULL, COL);
    int params_num = 0;
    while(params[params_num] != ";"){  // delim used at the end
        params_num++;
        params[params_num] = strtok(NULL,COL);
        cout<<params[params_num]<<endl;
        if(params[params_num] == ";")
            break;
    }

    for(int i = 0, j = 0; i < params_num/4 && j < params_num; i++){
        fromclient[i] = params[j++];
        ipfordownload[i] = params[j++];
        portfordownload[i] = atoi(params[j++].c_str());
        pathfordownload[i] = params[j++];
    }

   
    string filepathtodownload = strtok(NULL, ";"); 


    string tempmsg = "sendfile:" + pathfordownload[0] + ":";

    cout<<"temp msg is "<<tempmsg<<endl;

    int cli_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    cli_addr.sin_family = AF_INET;
    cli_addr.sin_addr.s_addr = inet_addr(ipfordownload[0].c_str());  // taking just one client for now
    cli_addr.sin_port = htons(portfordownload[0]);           // taking just one client for now
    
    int status = connect(cli_sockfd,(struct sockaddr *) &cli_addr,sizeof(cli_addr));

    if (status < 0)
        perror("Error in connecting");
    else
        printf("Connected...\n");

    send(cli_sockfd, tempmsg.c_str(), BUFFSIZE, 0);

            FILE *fp = fopen (filepathtodownload.c_str()  , "wb" );
            int fd = fileno(fp); 

    while ((n = recv(cli_sockfd , buffer , BUFFSIZE, 0) ) > 0  &&  filesize > 0){
                write(fd, buffer, n);
                memset ( buffer , '\0', n);
                filesize = filesize - n;
    } 

    cout<<"file download successful";

    fclose(fp);
    close(fd);

    close(cli_sockfd);
}


void * filegiver(void * threadarg){

    int newsockfd;
    struct sockaddr_in cli_addr,ser_addr;
    socklen_t clientlen = sizeof(cli_addr);

    int portno;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
        
    portno = thisclientPort;
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = inet_addr(thisclientIP.c_str());
    ser_addr.sin_port = htons(portno);
     
    bind(sockfd, (struct sockaddr *)&ser_addr, sizeof(ser_addr));

    listen(sockfd,5);

    while(1){


        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clientlen);
        char info[BUFFSIZE];
        recv(newsockfd, info, BUFFSIZE, 0);   // download dialoge with path and
        cout<<info<<endl;
        int n;
        string str = strtok(info,":");
        cout<<str<<"yeh toh aa gaya"<<endl;
        if(str == "sendfile"){
            string path = strtok(NULL,":");
            cout<<path<<endl;
            //cout<<"path"<<endl;
            FILE *fp = fopen(path.c_str(), "rb");
            int fd = fileno(fp); 
            if(fd < 0){
                perror("file not opened");
                exit(3);
            }

            fseek(fp, 0, SEEK_END);
            int size = ftell(fp);
            rewind(fp);
            char Buffer[BUFFSIZE]; 

            while (( n = fread( Buffer , sizeof(char) , BUFFSIZE , fp )) > 0  && size > 0 ){
                printf("%s \n", Buffer);
                send(newsockfd , Buffer, n, 0 );
                memset(Buffer , '\0', BUFFSIZE);
                size = size - n ;
            }

            fclose(fp);
        }
    }

    close(newsockfd);

}


/*client:
a. Run Client:
./client <IP>:<PORT> tracker_info.txt
tracker_info.txt - Contains ip, port details of all the trackers
b. Create User Account:
create_user <user_id> <passwd>
c. Login:
login <user_id> <passwd>
d. Create Group:
create_group <group_id>
e. Join Group:
join_group <group_id>
f. Leave Group:
leave_group <group_id>
g. List pending join requests
list_requests <group_id>
h. Accept Group Joining Request:
accept_request <group_id> <user_id>
i. List All Group In Network:
list_groups
j. List All sharable Files In Group:
list_files <group_id>
k. Upload File:
upload_file <file_path> <group_id>
l. Download File:
download_file <group_id> <file_name> <destination_path>
m. Logout:
logout
n. Show_downloads
Show_downloads
Output format:
[D] [grp_id] filename
[C] [grp_id] filename
D(Downloading), C(Complete)
o. Stop sharing
stop_share <group_id> <file_name*/

int main(int argc, char * argv[])
{
    
    string downldmsg;
    char * ipport = new char[50];
    ipport = argv[1];
    string ip = strtok(ipport,":");
    string port = strtok(NULL," ");
    thisclientIP = ip;                    
    thisclientPort = atoi(port.c_str());                  // setting the port and client for binding on this client
    cout<<ip<<endl;
    cout<<port<<endl;
    int i = 0;
    int portno;

    pthread_t serverlistnerThread;
    pthread_create(&serverlistnerThread, NULL, filegiver, NULL);

    while(1){

        string msg;
        int option = 0;
        cin>>msg;

        if (msg.compare("create_user") == 0) option = 1;
        else if (msg.compare("login") == 0) option = 2;
        else if (msg.compare("create_group") == 0) option = 3;
        else if (msg.compare("join_group") == 0) option = 4;
        else if (msg.compare("leave_group") == 0) option = 5;
        else if (msg.compare("list_requests") == 0) option = 6;
        else if (msg.compare("accept_request") == 0) option = 7;
        else if (msg.compare("list_groups") == 0) option = 8;   // server reply to be parsed
        else if (msg.compare("list_files") == 0) option = 9;  // server reply to be parsed
        else if (msg.compare("upload_file") == 0) option = 10;    // upload file info
        else if (msg.compare("download_file") == 0) option = 11;
        else if (msg.compare("logout") == 0) option = 12;
        else if (msg.compare("show_downloads") == 0) option = 13;
        else if (msg.compare("stop_share") == 0) option = 14;
        else{
            cout<< "Enter correct options please"<<endl;
            continue;
        }

        switch(option){
            case 1 :{ //cout<<msg<<" "<<option<<" ";
                cout<<"Input username and password for signup"<<endl;
                string username;
                string password;
                cin>>username;
                cin>>password;
                send_create_user(option, ip, port, username, password);
                break;
            }
            case 2 :{ //cout<<msg<<" "<<option<<" ";
                cout<<"Input username and password for login"<<endl;
                string username;
                string password;
                cin>>username;
                cin>>password;
                send_login_user(option, ip, port, username, password);

                break;
            }
            case 3 :{ //cout<<msg<<" "<<option<<" ";
                cout<<"Input groupid for creating a group(enter an integer)"<<endl;
                int groupid;
                cin>>groupid;
                send_group_create_info(option, ip, port, groupid);
                break;
            }
            case 4 :{// cout<<msg<<" "<<option<<" ";
                cout<<"Input groupid for joining a group(enter an integer)"<<endl;
                int groupid;
                cin>>groupid;
                send_group_join_info(option, ip, port, groupid);
                break;
            }
            case 5 : {//cout<<msg<<" "<<option<<" ";
                cout<<"Input groupid for leaving a group(enter an integer)"<<endl;
                int groupid;
                cin>>groupid;
                send_leave_group_info(option, ip, port, groupid);
                break;
            }
            case 6 :{ //cout<<msg<<" "<<option<<" ";
                //list_requests <group_id>
                cout<<"Input groupid for listing requests for a group(enter an integer)"<<endl;
                int groupid;
                cin>>groupid;
                send_list_request_group_info(option, ip, port, groupid);
                break;
            }
            case 7 : {// cout<<msg<<" "<<option<<" ";
                //accept_request <group_id> <user_id>
                cout<<"Input groupid and userid for accepting requests for a group"<<endl;
                int groupid;
                int userid;
                cin>>groupid>>userid;
                send_accept_request_group_info(option, ip, port, groupid, userid);
                break;
            }
            case 8 :{ //cout<<msg<<" "<<option<<" ";
                // list_groups
                send_list_all_group_info(option, ip, port);
                break;
            }
            case 9 :{ //cout<<msg<<" "<<option<<" ";
                //list_files <group_id>
                cout<<"Input groupid for listing files"<<endl;
                int groupid;
                cin>>groupid;
                send_groupid_for_listing_files_info(option, ip, port, groupid);
                break;
            }
            case 10 :{ //cout<<msg<<" "<<option<<" ";
                //upload_file <file_path> <group_id>
                cout<<"Input the file path filename and group id for uploading the file"<<endl;
                int groupid;
                string path;
                string fname;
                cin>>path;
                cin>>fname;
                cin>>groupid;
                send_upload_file_info(option, ip, port, path, fname, groupid);
                break;
            }
            case 11 : {//cout<<msg<<" "<<option<<" ";
                //download_file <group_id> <file_name> <destination_path>
                cout<<"Input the groupid, file name and destination path for downloading the file"<<endl;
                int groupid;
                string spath, dpath;
                cin>>groupid;
                cin>>spath;
                cin>>dpath;
                downldmsg = send_download_file_info(option, ip, port, spath, dpath, groupid);
                if(downldmsg.substr(0,4) == "down"){
                    downldmsg = downldmsg + ";:" + dpath + ";" ; // add destn path
                    cout<<"here i am dear"<<downldmsg;
                    downloadactualfile(downldmsg);   
                }
                
                // get some info and go to the thread for getting file from the info obtained
                break;
            }
            case 12 :{// cout<<msg<<" "<<option<<" ";
                //logout
                send_logout_info(option, ip, port);
                break;
            }
            case 13 : {//cout<<msg<<" "<<option<<" ";
                //Show_downloads
                send_show_downloads_info(option, ip, port);
                break;
            }
            case 14 : {cout<<msg<<" "<<option<<" ";
                //stop_share <group_id> <file_name
                int groupid;
                string filename;
                cin>>groupid;
                cin>>filename;
                send_stop_share_info(option, ip, port, groupid, filename);
                break;
            }
            default : break; 

        }

        if(option==11){
        // pthread_t serverlistnerThread;
        // pthread_create(&serverlistnerThread, NULL, serverlistner, &downldmsg);
        // pthread_join(serverlistnerThread, NULL);
        if(downldmsg == "no:"){
            cout<<"tracker has no info for file.";
            continue;
        }
        // else if(downldmsg.substr(0,4) == "down"){
        //     pthread_t clientlistnerThread[10];
        //     int thread_status = pthread_create(&clientlistnerThread[i], NULL, serverlistner, NULL);
        //     if(thread_status != 0){
        //         cout<<"thread creation failed"<<endl;
        //     } 
        //     if( i >= 3)
        //         {
        //             i = 0;
        //             while(i < 3)
        //             {
        //                     pthread_join(clientlistnerThread[i++],NULL);
        //             }
        //             i = 0;
        //         }
        //     }
        //     else{
        //         continue;
        //     }
        }

    }

    pthread_join(serverlistnerThread, NULL);
    close(sockfd);
    
    return 0;
}

