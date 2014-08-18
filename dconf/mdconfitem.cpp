/***************************************************************************
** This file was derived from the MDesktopEntry implementation in the
** libmeegotouch library.
**
** Original Copyright:
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
**
** Copyright on new work:
** Copyright 2011 Intel Corp.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

// This has to be the first include otherwise gdbusintrospection.h causes an error.
extern "C" {
#include <dconf/dconf.h>
};

#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QVariant>
#include <QDebug>

#include "mdconfitem.h"

#include "mdconf_p.h"

struct MDConfItemPrivate {
    MDConfItemPrivate() :
        client(dconf_client_new()),
	handler(0)
    {}

    QString key;
    QVariant value;
    DConfClient *client;
    gulong handler;
    static void notify_trampoline(DConfClient *, gchar *, GStrv, gchar *, gpointer);
};

static QByteArray convertKey(const QString &key)
{
    if (key.startsWith('/'))
        return key.toUtf8();
    else {
        QString replaced = key;
        replaced.replace('.', '/');
        qWarning() << "Using dot-separated key names with MGConfItem is deprecated.";
        qWarning() << "Please use" << '/' + replaced << "instead of" << key;
        return '/' + replaced.toUtf8();
    }
}

static QString convertKey(const char *key)
{
    return QString::fromUtf8(key);
}

void MDConfItemPrivate::notify_trampoline(DConfClient *, gchar *, GStrv, gchar *, gpointer data)
{
    MDConfItem *item = (MDConfItem *)data;
    item->update_value(true);
}

void MDConfItem::update_value(bool emit_signal)
{
    QVariant new_value;
    QByteArray k = convertKey(priv->key);
    GVariant *v = dconf_client_read(priv->client, k.data());
    if (!v) {
        qWarning() << "MGConfItem Failed to read" << priv->key;
        new_value = priv->value;
    }

    new_value = MDConf::convertValue(v);
    if (v)
        g_variant_unref(v);

    if (new_value != priv->value || new_value.userType() != priv->value.userType()) {
        priv->value = new_value;
        if (emit_signal)
            emit valueChanged();
    }
}

QString MDConfItem::key() const
{
    return priv->key;
}

QVariant MDConfItem::value() const
{
    return priv->value;
}

QVariant MDConfItem::value(const QVariant &def) const
{
    if (priv->value.isNull())
        return def;
    else
        return priv->value;
}

void MDConfItem::set(const QVariant &val)
{
    QByteArray k = convertKey(priv->key);
    GVariant *v = NULL;
    GError *error = NULL;

    if (MDConf::convertValue(val, &v)) {
	dconf_client_write_fast(priv->client, k.data(), v, &error);

	if (error) {
	    qWarning() << error->message;
	    g_error_free(error);
	}

	// We will not emit any signals because dconf should take care of that for us.
    } else
        qWarning() << "Can't store a" << val.typeName();
}

void MDConfItem::unset()
{
    set(QVariant());
}

QStringList MDConfItem::listDirs() const
{
    QStringList children;
    gint length = 0;
    QByteArray k = convertKey(priv->key);
    if (!k.endsWith("/")) {
        k = k.left(k.lastIndexOf("/") + 1);
    }
    qDebug() << "MDConfItem::listDirs" << k;

    gchar **dirs = dconf_client_list(priv->client, k.data(), &length);
    GError *error = NULL;

    for (gint x = 0; x < length; x++) {
      const gchar *dir = g_strdup_printf ("%s%s", k.data(), dirs[x]);
      if (dconf_is_dir(dir, &error)) {
        // We have to mimic how gconf was behaving.
        // so we need to chop off trailing slashes.
        // dconf will also barf if it gets a "path" with 2 slashes.
        QString d = convertKey(dir);
        g_free ((gpointer)dir);
        qDebug() << "have dir:" << d;
        if (d.endsWith("/")) {
          d.chop(1);
        }
        children.append(d);
      }

      // If we have error set then dconf_is_dir() has returned false so we should be safe here
      if (error) {
        qDebug() << "have not a dir:" << convertKey(dir);
        g_free ((gpointer)dir);
        qWarning() << "MGConfItem::listDirs()" << error->message;
        g_error_free(error);
        error = NULL;
      }
    }

    g_strfreev(dirs);

    return children;
}

QVariantMap MDConfItem::listItems() const
{

    QVariantMap children;
    gint length = 0;
    QByteArray k = convertKey(priv->key);
    if (!k.endsWith("/")) {
        k = k.left(k.lastIndexOf("/") + 1);
    }
    qDebug() << "MDConfItem::listItems" << k;

    gchar **items = dconf_client_list(priv->client, k.data(), &length);
    GError *error = NULL;

    for (gint x = 0; x < length; x++) {
      const gchar *item = g_strdup_printf ("%s%s", k.data(), items[x]);
      if (dconf_is_key(item, &error)) {
        // We have to mimic how gconf was behaving.
        // so we need to chop off trailing slashes.
        // dconf will also barf if it gets a "path" with 2 slashes.
        QString k = convertKey(item);
        QVariant val;
        GVariant *v = dconf_client_read(priv->client, item);
        if (!v) {
            qWarning() << "MGConfItem Failed to read" << priv->key;
            val = priv->value;
        }

        val = MDConf::convertValue(v);

        children[k] = val;
        qDebug() << "have item:" << k;

        if (v)
            g_variant_unref(v);
        g_free ((gpointer)item);
      }

      // If we have error set then dconf_is_key() has returned false so we should be safe here
      if (error) {
        qDebug() << "have not an item:" << convertKey(item);
        g_free ((gpointer)item);
        qWarning() << "MGConfItem::listItems()" << error->message;
        g_error_free(error);
        error = NULL;
      }
    }

    g_strfreev(items);

    return children;
}

bool MDConfItem::sync()
{
    dconf_client_sync(priv->client);
    return true;
}

MDConfItem::MDConfItem(const QString &key, QObject *parent)
    : QObject(parent)
{
    priv = new MDConfItemPrivate;
    priv->key = key;
    priv->handler =
      g_signal_connect(priv->client, "changed", G_CALLBACK(MDConfItemPrivate::notify_trampoline), this);

    QByteArray k = convertKey(priv->key);
    dconf_client_watch_fast(priv->client, k.data());
    update_value(false);
}

MDConfItem::~MDConfItem()
{
    g_signal_handler_disconnect(priv->client, priv->handler);
    QByteArray k = convertKey(priv->key);
    dconf_client_unwatch_fast(priv->client, k.data());
    g_object_unref(priv->client);
    delete priv;
}
