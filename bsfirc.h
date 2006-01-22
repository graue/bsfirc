#include "irclib/irclib.h"

#include "smartall.h"
#include "alloc.h"

#ifdef TIMESTAMPS
#define TIMESTAMPS_CHANMSG
#endif

enum {
	LAST_MESSAGE_NONE,
	LAST_MESSAGE_CHANNEL,
	LAST_MESSAGE_PRIVATE
};

struct BSFirc {
	void *handle;
	char *lastmsg;
	int lastmsgtype;
	char *lastchan;
	char *nick;
	char *server;
	int istyping;
	int ready;
};

struct Waiting {
	char *nick;
	struct Waiting *next;
};

struct UserList {
	char *name;
	uint8_t mode;
	struct UserList *next;
};

#define MODE_NONE  0x00
#define MODE_VOICE 0x01
#define MODE_OP    0x02

struct ChannelList {
	char *chan;
	struct UserList *users;
	struct ChannelList *next;
};

enum {
	EVENT_PRIVMSG,
	EVENT_CHANJOIN,
	EVENT_CHANPART,
	EVENT_CHANMSG,
	EVENT_QUIT,
	EVENT_SENDPRIVMSG
};

#define BSF_PROMPT "::"
#define USERLIST_ECHOSTR "  "

#include "protos.h"
