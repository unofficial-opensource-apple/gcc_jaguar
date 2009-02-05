/* Hash tables.
   Copyright (C) 2000, 2001 Free Software Foundation, Inc.

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

 In other words, you are welcome to use, share and improve this program.
 You are forbidden to forbid anyone else to use, share and improve
 what you give them.   Help stamp out software-hoarding!  */

#include "config.h"
#include "system.h"
#include "hashtable.h"

/* The code below is a specialization of Vladimir Makarov's expandable
   hash tables (see libiberty/hashtab.c).  The abstraction penalty was
   too high to continue using the generic form.  This code knows
   intrinsically how to calculate a hash value, and how to compare an
   existing entry with a potential new one.  Also, the ability to
   delete members from the table has been removed.  */

static unsigned int calc_hash PARAMS ((const unsigned char *, unsigned int));
static void ht_expand PARAMS ((hash_table *));

/* APPLE LOCAL PFE */
#ifdef PFE
#include "pfe/pfe.h"
/* Use PFE memory management routines instead of xmalloc & free.  */
#define OBSTACK_CHUNK_ALLOC pfe_obstack_chuck_alloc
#define OBSTACK_CHUNK_FREE pfe_free
static void *pfe_obstack_chuck_alloc PARAMS ((size_t));
static void *
pfe_obstack_chuck_alloc (size)
     size_t size;
{
  return PFE_MALLOC (size, PFE_ALLOC_OBSTACK_CHUNK);
}
#endif /* PFE */

/* Let particular systems override the size of a chunk.  */
#ifndef OBSTACK_CHUNK_SIZE
#define OBSTACK_CHUNK_SIZE 0
#endif
  /* Let them override the alloc and free routines too.  */
#ifndef OBSTACK_CHUNK_ALLOC
#define OBSTACK_CHUNK_ALLOC xmalloc
#endif
#ifndef OBSTACK_CHUNK_FREE
#define OBSTACK_CHUNK_FREE free
#endif

/* Initialise an obstack.  */
void
gcc_obstack_init (obstack)
     struct obstack *obstack;
{
  _obstack_begin (obstack, OBSTACK_CHUNK_SIZE, 0,
		  (void *(*) PARAMS ((long))) OBSTACK_CHUNK_ALLOC,
		  (void (*) PARAMS ((void *))) OBSTACK_CHUNK_FREE);
}

/* Calculate the hash of the string STR of length LEN.  */

static unsigned int
calc_hash (str, len)
     const unsigned char *str;
     unsigned int len;
{
  unsigned int n = len;
  unsigned int r = 0;
#define HASHSTEP(r, c) ((r) * 67 + ((c) - 113));

  while (n--)
    r = HASHSTEP (r, *str++);

  return r + len;
#undef HASHSTEP
}

/* Initialize an identifier hashtable.  */

hash_table *
ht_create (order)
     unsigned int order;
{
  unsigned int nslots = 1 << order;
  hash_table *table;

  /* APPLE LOCAL PFE - expand to pfe_malloc or xmalloc  */
  table = (hash_table *) PFE_MALLOC (sizeof (hash_table), PFE_ALLOC_HASH_TABLE);
  memset (table, 0, sizeof (hash_table));

  /* Strings need no alignment.  */
  gcc_obstack_init (&table->stack);
  obstack_alignment_mask (&table->stack) = 0;

  /* APPLE LOCAL PFE */
#ifdef PFE
  /* We need string alignment to avoid odd pointers.  */
  obstack_alignment_mask (&table->stack) = 1;
#endif

  /* APPLE LOCAL PFE - expand to pfe_calloc or xcalloc  */
  table->entries = (hashnode *) PFE_CALLOC (nslots, sizeof (hashnode),
  					    PFE_ALLOC_HASHNODE);
  table->nslots = nslots;
  return table;
}

/* Frees all memory associated with a hash table.  */

void
ht_destroy (table)
     hash_table *table;
{
  obstack_free (&table->stack, NULL);
  free (table->entries);
  free (table);
}

/* Returns the hash entry for the a STR of length LEN.  If that string
   already exists in the table, returns the existing entry, and, if
   INSERT is CPP_ALLOCED, frees the last obstack object.  If the
   identifier hasn't been seen before, and INSERT is CPP_NO_INSERT,
   returns NULL.  Otherwise insert and returns a new entry.  A new
   string is alloced if INSERT is CPP_ALLOC, otherwise INSERT is
   CPP_ALLOCED and the item is assumed to be at the top of the
   obstack.  */
hashnode
ht_lookup (table, str, len, insert)
     hash_table *table;
     const unsigned char *str;
     unsigned int len;
     enum ht_lookup_option insert;
{
  unsigned int hash = calc_hash (str, len);
  unsigned int hash2;
  unsigned int index;
  size_t sizemask;
  hashnode node;

  sizemask = table->nslots - 1;
  index = hash & sizemask;

  /* hash2 must be odd, so we're guaranteed to visit every possible
     location in the table during rehashing.  */
  hash2 = ((hash * 17) & sizemask) | 1;
  table->searches++;

  for (;;)
    {
      node = table->entries[index];

      if (node == NULL)
	break;

      if (HT_LEN (node) == len && !memcmp (HT_STR (node), str, len))
	{
	  if (insert == HT_ALLOCED)
	    /* The string we search for was placed at the end of the
	       obstack.  Release it.  */
	    obstack_free (&table->stack, (PTR) str);
	  return node;
	}

      index = (index + hash2) & sizemask;
      table->collisions++;
    }

  if (insert == HT_NO_INSERT)
    return NULL;

  node = (*table->alloc_node) (table);
  table->entries[index] = node;

  HT_LEN (node) = len;
  if (insert == HT_ALLOC)
    HT_STR (node) = obstack_copy0 (&table->stack, str, len);
  else
    HT_STR (node) = str;

  if (++table->nelements * 4 >= table->nslots * 3)
    /* Must expand the string table.  */
    ht_expand (table);

  return node;
}

/* Double the size of a hash table, re-hashing existing entries.  */

static void
ht_expand (table)
     hash_table *table;
{
  hashnode *nentries, *p, *limit;
  unsigned int size, sizemask;

  size = table->nslots * 2;
  /* APPLE LOCAL PFE - expand to pfe_calloc or xcalloc  */
  nentries = (hashnode *) PFE_CALLOC (size, sizeof (hashnode), PFE_ALLOC_HASHNODE);
  sizemask = size - 1;

  p = table->entries;
  limit = p + table->nslots;
  do
    if (*p)
      {
	unsigned int index, hash, hash2;

	hash = calc_hash (HT_STR (*p), HT_LEN (*p));
	hash2 = ((hash * 17) & sizemask) | 1;
	index = hash & sizemask;

	for (;;)
	  {
	    if (! nentries[index])
	      {
		nentries[index] = *p;
		break;
	      }

	    index = (index + hash2) & sizemask;
	  }
      }
  while (++p < limit);

  /* APPLE LOCAL PFE - expand to pfe_free or free  */
  PFE_FREE (table->entries);
  table->entries = nentries;
  table->nslots = size;
}

/* For all nodes in TABLE, callback CB with parameters TABLE->PFILE,
   the node, and V.  */
void
ht_forall (table, cb, v)
     hash_table *table;
     ht_cb cb;
     const PTR v;
{
  hashnode *p, *limit;

  p = table->entries;
  limit = p + table->nslots;
  do
    if (*p)
      {
	if ((*cb) (table->pfile, *p, v) == 0)
	  break;
      }
  while (++p < limit);
}

/* Dump allocation statistics to stderr.  */

void
ht_dump_statistics (table)
     hash_table *table;
{
  size_t nelts, nids, overhead, headers;
  size_t total_bytes, longest, sum_of_squares;
  double exp_len, exp_len2, exp2_len;
  hashnode *p, *limit;

#define SCALE(x) ((unsigned long) ((x) < 1024*10 \
		  ? (x) \
		  : ((x) < 1024*1024*10 \
		     ? (x) / 1024 \
		     : (x) / (1024*1024))))
#define LABEL(x) ((x) < 1024*10 ? ' ' : ((x) < 1024*1024*10 ? 'k' : 'M'))

  total_bytes = longest = sum_of_squares = nids = 0;
  p = table->entries;
  limit = p + table->nslots;
  do
    if (*p)
      {
	size_t n = HT_LEN (*p);

	total_bytes += n;
	sum_of_squares += n * n;
	if (n > longest)
	  longest = n;
	nids++;
      }
  while (++p < limit);
      
  nelts = table->nelements;
  overhead = obstack_memory_used (&table->stack) - total_bytes;
  headers = table->nslots * sizeof (hashnode);

  fprintf (stderr, "\nString pool\nentries\t\t%lu\n",
	   (unsigned long) nelts);
  fprintf (stderr, "identifiers\t%lu (%.2f%%)\n",
	   (unsigned long) nids, nids * 100.0 / nelts);
  fprintf (stderr, "slots\t\t%lu\n",
	   (unsigned long) table->nslots);
  fprintf (stderr, "bytes\t\t%lu%c (%lu%c overhead)\n",
	   SCALE (total_bytes), LABEL (total_bytes),
	   SCALE (overhead), LABEL (overhead));
  fprintf (stderr, "table size\t%lu%c\n",
	   SCALE (headers), LABEL (headers));

  exp_len = (double)total_bytes / (double)nelts;
  exp2_len = exp_len * exp_len;
  exp_len2 = (double) sum_of_squares / (double) nelts;

  fprintf (stderr, "coll/search\t%.4f\n",
	   (double) table->collisions / (double) table->searches);
  fprintf (stderr, "ins/search\t%.4f\n",
	   (double) nelts / (double) table->searches);
  fprintf (stderr, "avg. entry\t%.2f bytes (+/- %.2f)\n",
	   exp_len, approx_sqrt (exp_len2 - exp2_len));
  fprintf (stderr, "longest entry\t%lu\n",
	   (unsigned long) longest);
#undef SCALE
#undef LABEL
}

/* Return the approximate positive square root of a number N.  This is for
   statistical reports, not code generation.  */
double
approx_sqrt (x)
     double x;
{
  double s, d;

  if (x < 0)
    abort ();
  if (x == 0)
    return 0;

  s = x;
  do
    {
      d = (s * s - x) / (2 * s);
      s -= d;
    }
  while (d > .0001);
  return s;
}
