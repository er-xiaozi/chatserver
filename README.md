# chatserver
集群聊天服务器，muduo+mysql+redis+nginx

### 一、环境配置

1、更新软件包

```
sudo apt-get update
sudo apt-get upgrade

sudo apt install openssh-server
sudo systemctl start ssh
sudo systemctl enable ssh
sudo systemctl restart ssh
```

2、安装依赖

```
sudo apt-get install -y git cmake g++ libboost-all-dev libssl-dev
```

- **git**：用于克隆 Muduo 的源码仓库。
- **cmake**：用于构建项目。
- **g++**：C++ 编译器。
- **libboost-all-dev**：Boost 库，Muduo 依赖于 Boost。
- **libssl-dev**：用于 SSL 支持（如果需要）。

3、克隆 [Muduo](https://so.csdn.net/so/search?q=Muduo&spm=1001.2101.3001.7020) 的源码仓库

```
git clone https://github.com/chenshuo/muduo.git
```

##### 4. 创建构建目录并编译

进入 Muduo 源码目录，创建一个构建目录，并使用 CMake 进行编译。

```bash
cd muduo
mkdir build
cd build
cmake ..
make
```

##### 5. 安装 Muduo

如果你希望将 Muduo 安装到系统目录，可以执行以下命令：

```bash
sudo make install
```

默认情况下，Muduo 会被安装到 /usr/local 目录下。如果需要自定义安装路径，可以在运行 cmake 时指定 CMAKE_INSTALL_PREFIX：

```
cmake -DCMAKE_INSTALL_PREFIX=/your/custom/path ..
make
sudo make install
```

##### 6. 配置环境变量（如果需要）

如果你将 Muduo 安装到了自定义路径，可能需要将库路径添加到 LD_LIBRARY_PATH 环境变量中。

```
export LD_LIBRARY_PATH=/your/custom/path/lib:$LD_LIBRARY_PATH
```

你可以将上述命令添加到 ~/.bashrc 或 ~/.profile 文件中，以便每次启动终端时自动设置。

##### 7. 验证安装

你可以编译并运行 Muduo 提供的示例程序，确保安装成功。

```bash
cd ../examples
make
./echo_server
123
```

如果 `echo_server` 启动成功并监听指定端口，说明 Muduo 安装成功。

#### 配置muduo

```
编译
g++ -o server muduo_server.cpp -lmuduo_net -lmuduo_base -lpthread
```

按住Ctrl+Shift+B,  点C/C++ :生成活动文件后面的小齿轮，进入tesk.json

添加连接库

```
"args": [
				"-fdiagnostics-color=always",
				"-g",
				"${file}",
				"-o",
				"${fileDirname}/${fileBasenameNoExtension}",
				"-lmuduo_net",
				"-lmuduo_base",
				"-lpthread"
			],
```

#### CMake使用

```
sudo apt install cmake
cmake -version
```

#### MySQL

安装mysql-server和libmysqlclient-dev开发包

````
sudo apt-get install mysql-server
sudo apt-get install libmysqlclient-dev
````

查看初始用户账户密码

```
lwj@iZbp14ynxnzb8tz3hajyrbZ:~$ sudo cat /etc/mysql/debian.cnf
# Automatically generated for Debian scripts. DO NOT TOUCH!
[client]
host     = localhost
user     = debian-sys-maint
password = U7kphTFPNWbw8diJ
socket   = /var/run/mysqld/mysqld.sock
[mysql_upgrade]
host     = localhost
user     = debian-sys-maint
password = 9ZmUEysDHuauWNY8
socket   = /var/run/mysqld/mysqld.sock
```

修改初始用户名和密码

```
mysql -u debian-sys-maint -pU7kphTFPNWbw8diJ

-u后面是用户名 -p后面紧跟密码

修改root用户的密码为123456
下面是mysql8.0以前的做法
mysql> update mysql.user set authentication_string=PASSWORD('123456') where user='root' and host='localhost'
mysql> update mysql.user set plugin='mysql_native_password' 
mysql> FLUSH PRIVILEGES;

下面是mysql8.0以后的做法
修改root密码
use mysql;
select user,plugin from user;
update user set plugin='mysql_native_password' where user='root'; # 修改其密码格式
select user,plugin from user; # 查询其用户
flush privileges;
ALTER USER 'root'@'localhost' IDENTIFIED BY '123456';
FLUSH PRIVILEGES;
exit
service mysql restart
mysql -u root -p
```

设置MySQL字符编码为utf-8，支持中文存储

```
mysql> show variables like "char%";
```

建表

```
CREATE DATABASE chat;

CREATE TABLE user(
id INT UNSIGNED PRIMARY KEY AUTO_INCREMENT COMMENT '用户的id',
name VARCHAR(50) UNIQUE NOT NULL COMMENT '用户名',
password VARCHAR(50) NOT NULL COMMENT '用户密码',
state ENUM('online','offline') DEFAULT "offline" COMMENT '登录状态');

CREATE TABLE friend(
userid INT NOT NULL COMMENT '用户id',
friendid INT NOT NULL COMMENT '好友id',
PRIMARY KEY (userid, friendid));

CREATE TABLE allgroup(
id INT PRIMARY KEY AUTO_INCREMENT COMMENT '组id',
groupname VARCHAR(50) NOT NULL COMMENT '组名称',
groupdesc VARCHAR(200) DEFAULT '' COMMENT '组功能描述');

CREATE TABLE groupuser(
groupid INT PRIMARY KEY COMMENT '组id',
userid INT NOT NULL COMMENT '组员id',
grouprole ENUM('creator','normal') DEFAULT 'normal' COMMENT '组内角色');

CREATE TABLE offlinemessage(
userid INT NOT NULL COMMENT '用户id',
message VARCHAR(500) NOT NULL COMMENT '离线消息(存储Json字符串)');
```

```
use chat;
desc user;

show tables;


添加表项
ALTER TABLE user 
    ADD COLUMN email VARCHAR(100);
删除表项
ALTER TABLE user 
    DROP COLUMN age,
    DROP COLUMN phone;
修改表项
ALTER TABLE user 
    CHANGE COLUMN name username VARCHAR(50) NOT NULL;
查看表项
DESC user;
```

##### 表设计

**第一范式（1NF）** 

```
 telnet 127.0.0.1 6000
```

### 测试

```
telnet 127.0.0.1 6000
{"msgid":3,"name":"li si","password":"6666666"}//注册
{"msgid":1,"id":1,"password":"123456"} //登录
{"msgid":1,"id":2,"password":"6666666"}
//改状态
update user set state='offline' where id=1;

{"msgid":3,"name":"zhang san","password":"123456"}

聊天消息
{"msgid":5,"id":1,"from":"zhang san","to":2,"msg":"hello22!"}
{"msgid":5,"id":2,"from":"li si","to":1,"msg":"你好!"}
msgid

friend和user表联合查询
select a.id,a.name,a.state from user a inner join friend b on b.friendid = a.id where b.userid=%d

添加好友
{"msgid":6,"id":1,"friendid":2}
```

### 二、C++ 知识
