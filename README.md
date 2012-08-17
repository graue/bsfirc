# bsfirc

bsfirc is a lightweight, line-based and very portable IRC client,
originally written by Claudio Leite. In 2005 I took his (never
officially released) bsfirc code out of the CVS repository for
[bsflite](http://bsflite.sourceforge.net/) and started improving it,
making it my IRC client of choice.

These days I rarely go on IRC, but when I do, I use bsfirc.

It's controlled with this set of one-character commands:

    j<chan>        : join <chan>
    p<chan>        : leave <chan>
    m<rec> <msg>   : send <msg> to <rec> (channel or user)
    c<msg>         : reply to last channel message
    '<msg>         : same as c
    [TAB]<msg>     : reply to last private message
    r<msg>         : reply to last message
    w              : show who is in the channel
    i<nick>        : whois <nick>
    W              : show your nickname and server
    t<msg>         : set topic on current channel
    a<msg>         : set away message
    o<nick>        : op <nick> on current channel
    n<nick>        : change your nick to <nick>
    q!             : quit
    h              : show this command list

## License

bsfirc uses the 3-clause "modified BSD" license. I've dedicated my
own changes (most of which are pretty small) to the public domain.
