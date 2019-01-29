I have [this C++ code file](https://github.com/shlomif/fc-solve/blob/9812d6688c576631653b2fdd25fa9affed870718/fc-solve/source/google_hash.cpp#L137)
which is [FOSS](https://en.wikipedia.org/wiki/Free_and_open-source_software)
under the Expat licence. When running [cppcheck](http://cppcheck.sourceforge.net/)
on the code I get this error:

`[google_hash.cpp:137] -> [google_hash.cpp:141]: (error) Iterator 'it' used after element has been erased.`

The code in question is:

```
    74	#if (FCS_WHICH_STATES_GOOGLE_HASH == FCS_WHICH_STATES_GOOGLE_HASH__SPARSE)
    75	typedef sparse_hash_set<char *, state_hash, state_equality> StatesGoogleHash;
    76	#else
    77	typedef dense_hash_set<char *, state_hash, state_equality> StatesGoogleHash;
    78	#endif
.
.
.
   131	extern void fc_solve_states_google_hash_foreach(
   132	    fcs_states_google_hash_handle void_hash,
   133	    bool (*should_delete_ptr)(void *key, void *context), void *context)
   134	{
   135	    StatesGoogleHash *hash = (StatesGoogleHash *)void_hash;
   136
   137	    for (StatesGoogleHash::iterator it = hash->begin(); it != hash->end(); ++it)
   138	    {
   139	        if (should_delete_ptr(*(it), context))
   140	        {
   141	            hash->erase(it);
   142	        }
   143	    }
   144	}
```

It makes use of either
http://goog-sparsehash.sourceforge.net/doc/sparse_hash_set.html
or http://goog-sparsehash.sourceforge.net/doc/dense_hash_set.html .

Now, these documents say that

```
Validity of Iterators

insert() invalidates all iterators, as does resize().
erase() is guaranteed not to invalidate any iterators.
```

So my question is - am I using the sets' iterators correctly and safely
(and cppcheck has emitted a false positive), or if not - how should the
code be fixed?

Help will be appreciated.
