using namespace std;
class MailRecord
{
public:
    MailRecord ( const CMail & mail, long long seq )
            : m_Mail(mail), m_Seq(seq)
    {
    }
    const CMail & mail () const
    {
        return m_Mail;
    }
    long long seq () const
    {
        return m_Seq;
    }
    bool operator < ( const MailRecord & other ) const
    {
        int cmp = m_Mail.timeStamp().compare( other.m_Mail.timeStamp() );
        if ( cmp != 0 )
            return cmp < 0;
        return m_Seq < other.m_Seq;
    }
private:
    CMail m_Mail;
    long long m_Seq;
};

class CMailLog
{
public:
    int parseLog(istream & in)
    {
        int deliveredCount = 0;
        string line;
        unordered_map<string, pair<string, optional<string>>> temp;
        while (getline(in, line))
        {
            if (line.empty())
                continue;

            istringstream iss(line);
            string monthStr;
            int day, year;
            string timeStr, relay, mailIDToken;

            if (!(iss >> monthStr >> day >> year >> timeStr >> relay >> mailIDToken))
                continue;

            if (mailIDToken.back() != ':')
                continue;

            string mailID = mailIDToken.substr(0, mailIDToken.size() - 1);
            string message;
            getline(iss, message);
            size_t pos = message.find_first_not_of(" ");

            if (pos != string::npos)
                message = message.substr(pos);
            else
                message = "";

            const string fromPrefix    = "from=";
            const string subjectPrefix = "subject=";
            const string toPrefix      = "to=";
            bool isFrom    = (message.compare(0, fromPrefix.size(), fromPrefix) == 0);
            bool isSubject = (message.compare(0, subjectPrefix.size(), subjectPrefix) == 0);
            bool isTo      = (message.compare(0, toPrefix.size(), toPrefix) == 0);

            if (!isFrom && !isSubject && !isTo)
                continue;

            int month = 0;

            if( monthStr == "Jan" ) month = 1;
            else if ( monthStr == "Feb" ) month = 2;
            else if ( monthStr == "Mar" ) month = 3;
            else if ( monthStr == "Apr" ) month = 4;
            else if ( monthStr == "May" ) month = 5;
            else if ( monthStr == "Jun" ) month = 6;
            else if ( monthStr == "Jul" ) month = 7;
            else if ( monthStr == "Aug" ) month = 8;
            else if ( monthStr == "Sep" ) month = 9;
            else if ( monthStr == "Oct" ) month = 10;
            else if ( monthStr == "Nov" ) month = 11;
            else if ( monthStr == "Dec" ) month = 12;
            else
                continue;

            int hour, minute;
            double sec;
            if (sscanf(timeStr.c_str(), "%d:%d:%lf", &hour, &minute, &sec) != 3)
                continue;

            CTimeStamp ts(year, month, day, hour, minute, sec);
            if (isFrom)
            {
                string sender = message.substr(fromPrefix.size());
                temp[mailID] = make_pair(sender, optional<string>{});
            }
            else if (isSubject)
            {
                auto it = temp.find(mailID);
                if (it == temp.end() || it->second.first.empty())
                    continue;
                string subject = message.substr(subjectPrefix.size());
                it->second.second = subject;
            }
            else if (isTo)
            {
                auto it = temp.find(mailID);
                if (it == temp.end() || it->second.first.empty())
                    continue;
                string recipient = message.substr(toPrefix.size());
                CMail mail(ts, it->second.first, recipient, it->second.second);
                m_Mails.insert(MailRecord(mail, m_Seq++));
                deliveredCount++;
            }
        }
        temp.clear();
        return deliveredCount;
    }

    list<CMail> listMail(const CTimeStamp & from, const CTimeStamp & to) const
    {
        list<CMail> result;
        CMail kostyl(from, "", "", {});
        MailRecord kostylRec(kostyl, 0);
        auto it = m_Mails.lower_bound(kostylRec);
        for (; it != m_Mails.end(); ++it)
        {
            if (it->mail().timeStamp().compare(to) > 0)
                break;
            result.push_back(it->mail());
        }
        return result;
    }

    set<string> activeUsers(const CTimeStamp & from, const CTimeStamp & to) const
    {
        set<string> users;
        list<CMail> mails = listMail(from, to);
        for (const auto & mail : mails)
        {
            users.insert(mail.from());
            users.insert(mail.to());
        }
        return users;
    }

private:
    multiset<MailRecord> m_Mails;
    unordered_map<string, pair<string, optional<string>>> m_Temp;
    long long m_Seq = 0;
};
