use bbs;

drop table thread;
drop table file;
drop table comment;
create table thread (thread_id int not null auto_increment, title text not null, name text not null, time int not null, primary key(thread_id));
create table file (comment_id int not null, position int not null, save_name text not null, submit_name text not null, primary key(comment_id, position));
create table comment (comment_id int not null auto_increment, thread_id int not null, name text not null, time int not null, password text not null, comment text not null, primary key(comment_id));
