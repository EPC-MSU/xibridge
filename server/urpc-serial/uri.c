#include "uri.h"


static int parse_uri_after_path(
    const char *ppath,
    struct urpc_uri_t *result
)
{
    const char *p = ppath, *pn;

    // check is there a param
    pn = strstr(p, "?");

    if (pn)
    {
        // write path
        if ((size_t)(pn + 1 - p) > sizeof(result->path))
        {
            return 1;
        }
        memcpy(result->path, p, pn - p);
        result->path[pn - p] = 0;

        // parse param
        p = pn;
        pn = strstr(p, "=");
        if (pn)
        {
            // param name
            if ((size_t)(pn + 1 - p) > sizeof(result->paramname))
            {
                return 1;
            }
            memcpy(result->paramname, p + 1, pn - p - 1);
            result->paramname[pn - p - 1] = 0;
            // param value
            if (strlen(pn) + 1 > sizeof(result->paramvalue))
            {
                return 1;
            }
            strcpy(result->paramvalue, pn + 1);
            result->paramvalue[strlen(pn)] = 0;
        }
    }
    else
    {
        // path without param
        if (strlen(p) + 1 > sizeof(result->path))
        {
            return 1;
        }
        strcpy(result->path, p);
        result->path[strlen(p)] = 0;
    }
    return 0;
}

/* Parse an uri
 * Returns 0 on success
 */

int urpc_uri_parse(
    const char *uri,
    struct urpc_uri_t *result
)
{
    const char *p, *pn, *pm;

    p = uri + strspn(uri, " \t");
    if (!p)
    {
        return 1;
    }
    // parse scheme
    pn = strchr(p, ':');
    if (!pn)
    {
        return 1;
    }
    // copy scheme
    if ((size_t)(pn + 1 - p) > sizeof(result->scheme))
    {
        return 1;
    }
    memcpy(result->scheme, p, pn - p);
    result->scheme[pn - p] = 0;

    // move after colon
    p = pn + 1;

    // parse host
    if (strstr(p, "//") == p)
    {
        // there is a host part (i.e. scheme://host or scheme://host/path)
        p += 2;
        if (!*p)
        {
            return 1;
        }
        // p now points to first character after //
        pn = strchr(p, '/');
        if (pn == p)
        {
            // no host part, only path (scheme://path)
            if (parse_uri_after_path(p, result))
            {
                return 1;
            }
        }
        else if (pn)
        {
            // path part (scheme://host/path)
            if ((size_t)(pn + 1 - p) > sizeof(result->host))
            {
                return 1;
            }

            memcpy(result->host, p, pn - p);
            result->host[pn - p] = 0;

            if (parse_uri_after_path(pn + 1, result))
            {
                return 1;
            }
        }
        else // pn == NULL
        {
            // no path (scheme://host)

            if (strlen(p) + 1 > sizeof(result->host))
            {
                return 1;
            }

            pm = strchr(p, ':');
            if (pm)
            {
                // scheme://host:port
                memcpy(result->host, p, pm - p);
                result->host[pm - p] = 0;

                long int port = strtol(pm + 1, NULL, 10);
                if (!port)
                {
                    return 1;
                }
                result->port = (int)port;
            }
            else
            {
                // scheme://host
                strcpy(result->host, p);
                result->host[strlen(p)] = 0;
            }
        }
    }
    else
    {
        if (*p)
        {
            // only path (scheme:path)
            if (strlen(p) + 1 > sizeof(result->path))
            {
                return 1;
            }
            strcpy(result->path, p);
            result->path[strlen(p)] = 0;
        }
    }
    return 0;
}
