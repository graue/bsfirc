void error_callback (void *handle, int code);
char           * undo_netspeak (char *orig);
void get_input (void);
void parse_input (void);
void irc_topic (void *h, char *chan, char *topic);
void irc_motd (void *h, char *motd);
void irc_notice_auth (void *h, char *msg);
void irc_ready (void *h);
void irc_server_name (void *h, char *name);
void irc_join (void *h, char *nick, char *host, char *channel);
void irc_addchanuser (void *h, char *chan, char *nick);
void irc_namesdone (void *h, char *chan);
void irc_part (void *h, char *nick, char *host, char *channel);
void irc_quit (void *h, char *nick, char *msg);
void irc_nickinuse (void *h, char *nick);
void irc_ctcp (void *h, char *nick, char *host, char *target, char *msg);
void irc_mode (void *h, char *nick, char *host, char *target, int plus, int mode, char *arg);
void irc_notice (void *h, char *nick, char *host, char *msg);
void irc_nickchange (void *h, char *old, char *new);
void irc_msg (void *h, char *nick, char *host, char *target, char *msg);
void irc_whois_userhost (void *h, char *nick, char *user, char *host, char *name);
void irc_whois_channels (void *h, char *nick, char *channels);
void irc_whois_server (void *h, char *nick, char *server, char *serverdescr);
void irc_away (void *h, char *nick, char *awaymsg);
void free_lists (void);
void show_channel_users (char *chan);
void delete_channel_user (char *name, char *chan);
void process_quit (char *name, char *msg);
void change_user_mode (char *name, char *chan, int plus, int mode);
void change_user_nick (char *old, char *new);
void add_channel_user (char *name, char *chan, uint8_t mode);
int open_log_dir (void);
void log_event (int event_type, char *name, char *host, char *chan, char *msg);
void show_log (int lines, char *dest);
void addts (void);
void addts_short (void);
void eraseline (void);
void show_prompt (void);
int getwidth (void);
void sigwinch_handler (int a);
void setup_tty (void);
void restore_tty (void);
void get_screen_size (void);
void wordwrap_print (char *str, int offset);
void wordwrap_print_echostr (char *str, char *echostr);
