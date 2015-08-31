



void init(const char *user, const char *host)
{
	snprintf(user_name, sizeof(user_name), "%s", user);
	snprintf(host_name, sizeof(host_name), "%s", host);
	create_server();
	broad_cast_online_info();
}
