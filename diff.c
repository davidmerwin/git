#include "object-store.h"
static int diff_color_moved_ws_default;
	GIT_COLOR_FAINT,	/* CONTEXT_DIM */
	GIT_COLOR_FAINT_RED,	/* OLD_DIM */
	GIT_COLOR_FAINT_GREEN,	/* NEW_DIM */
	GIT_COLOR_BOLD,		/* CONTEXT_BOLD */
	GIT_COLOR_BOLD_RED,	/* OLD_BOLD */
	GIT_COLOR_BOLD_GREEN,	/* NEW_BOLD */
	[DIFF_CONTEXT_DIM]	      = "contextDimmed",
	[DIFF_FILE_OLD_DIM]	      = "oldDimmed",
	[DIFF_FILE_NEW_DIM]	      = "newDimmed",
	[DIFF_CONTEXT_BOLD]	      = "contextBold",
	[DIFF_FILE_OLD_BOLD]	      = "oldBold",
	[DIFF_FILE_NEW_BOLD]	      = "newBold",
	else if (!strcmp(arg, "blocks"))
		return COLOR_MOVED_BLOCKS;
	else if (!strcmp(arg, "dimmed-zebra"))
		return COLOR_MOVED_ZEBRA_DIM;
		return error(_("color moved setting must be one of 'no', 'default', 'blocks', 'zebra', 'dimmed-zebra', 'plain'"));
}

static int parse_color_moved_ws(const char *arg)
{
	int ret = 0;
	struct string_list l = STRING_LIST_INIT_DUP;
	struct string_list_item *i;

	string_list_split(&l, arg, ',', -1);

	for_each_string_list_item(i, &l) {
		struct strbuf sb = STRBUF_INIT;
		strbuf_addstr(&sb, i->string);
		strbuf_trim(&sb);

		if (!strcmp(sb.buf, "ignore-space-change"))
			ret |= XDF_IGNORE_WHITESPACE_CHANGE;
		else if (!strcmp(sb.buf, "ignore-space-at-eol"))
			ret |= XDF_IGNORE_WHITESPACE_AT_EOL;
		else if (!strcmp(sb.buf, "ignore-all-space"))
			ret |= XDF_IGNORE_WHITESPACE;
		else if (!strcmp(sb.buf, "allow-indentation-change"))
			ret |= COLOR_MOVED_WS_ALLOW_INDENTATION_CHANGE;
		else
			error(_("ignoring unknown color-moved-ws mode '%s'"), sb.buf);

		strbuf_release(&sb);
	}

	if ((ret & COLOR_MOVED_WS_ALLOW_INDENTATION_CHANGE) &&
	    (ret & XDF_WHITESPACE_FLAGS))
		die(_("color-moved-ws: allow-indentation-change cannot be combined with other white space modes"));

	string_list_clear(&l, 0);

	return ret;
	if (!strcmp(var, "diff.colormovedws")) {
		int cm = parse_color_moved_ws(value);
		if (cm < 0)
			return -1;
		diff_color_moved_ws_default = cm;
		return 0;
	}
static int fill_mmfile(struct repository *r, mmfile_t *mf,
		       struct diff_filespec *one)
	else if (diff_populate_filespec(r, one, 0))
static unsigned long diff_filespec_size(struct repository *r,
					struct diff_filespec *one)
	diff_populate_filespec(r, one, CHECK_SIZE_ONLY);
static void emit_line_0(struct diff_options *o,
			const char *set_sign, const char *set, unsigned reverse, const char *reset,
	int needs_reset = 0; /* at the end of the line */
	has_trailing_newline = (len > 0 && line[len-1] == '\n');
	if (has_trailing_newline)
		len--;

	has_trailing_carriage_return = (len > 0 && line[len-1] == '\r');
	if (has_trailing_carriage_return)
		len--;

	if (!len && !first)
		goto end_of_line;

	if (reverse && want_color(o->use_color)) {
		fputs(GIT_COLOR_REVERSE, file);
		needs_reset = 1;
	}

	if (set_sign) {
		fputs(set_sign, file);
		needs_reset = 1;
	if (first)
		fputc(first, file);

	if (!len)
		goto end_of_line;

	if (set) {
		if (set_sign && set != set_sign)
			fputs(reset, file);
		needs_reset = 1;
	fwrite(line, len, 1, file);
	needs_reset = 1; /* 'line' may contain color codes. */

end_of_line:
	if (needs_reset)
		fputs(reset, file);
	emit_line_0(o, set, NULL, 0, reset, 0, line, len);
/**
 * The struct ws_delta holds white space differences between moved lines, i.e.
 * between '+' and '-' lines that have been detected to be a move.
 * The string contains the difference in leading white spaces, before the
 * rest of the line is compared using the white space config for move
 * coloring. The current_longer indicates if the first string in the
 * comparision is longer than the second.
 */
struct ws_delta {
	char *string;
	unsigned int current_longer : 1;
};
#define WS_DELTA_INIT { NULL, 0 }

struct moved_block {
	struct moved_entry *match;
	struct ws_delta wsd;
};

static void moved_block_clear(struct moved_block *b)
{
	FREE_AND_NULL(b->wsd.string);
	b->match = NULL;
}

static int compute_ws_delta(const struct emitted_diff_symbol *a,
			     const struct emitted_diff_symbol *b,
			     struct ws_delta *out)
{
	const struct emitted_diff_symbol *longer =  a->len > b->len ? a : b;
	const struct emitted_diff_symbol *shorter = a->len > b->len ? b : a;
	int d = longer->len - shorter->len;

	if (strncmp(longer->line + d, shorter->line, shorter->len))
		return 0;

	out->string = xmemdupz(longer->line, d);
	out->current_longer = (a == longer);

	return 1;
}

static int cmp_in_block_with_wsd(const struct diff_options *o,
				 const struct moved_entry *cur,
				 const struct moved_entry *match,
				 struct moved_block *pmb,
				 int n)
{
	struct emitted_diff_symbol *l = &o->emitted_symbols->buf[n];
	int al = cur->es->len, cl = l->len;
	const char *a = cur->es->line,
		   *b = match->es->line,
		   *c = l->line;

	int wslen;

	/*
	 * We need to check if 'cur' is equal to 'match'.
	 * As those are from the same (+/-) side, we do not need to adjust for
	 * indent changes. However these were found using fuzzy matching
	 * so we do have to check if they are equal.
	 */
	if (strcmp(a, b))
		return 1;

	if (!pmb->wsd.string)
		/*
		 * The white space delta is not active? This can happen
		 * when we exit early in this function.
		 */
		return 1;

	/*
	 * The indent changes of the block are known and stored in
	 * pmb->wsd; however we need to check if the indent changes of the
	 * current line are still the same as before.
	 *
	 * To do so we need to compare 'l' to 'cur', adjusting the
	 * one of them for the white spaces, depending which was longer.
	 */

	wslen = strlen(pmb->wsd.string);
	if (pmb->wsd.current_longer) {
		c += wslen;
		cl -= wslen;
	} else {
		a += wslen;
		al -= wslen;
	}

	if (al != cl || memcmp(a, c, al))
		return 1;

	return 0;
}

static int moved_entry_cmp(const void *hashmap_cmp_fn_data,
			   const void *entry,
			   const void *entry_or_key,
	const struct diff_options *diffopt = hashmap_cmp_fn_data;
	const struct moved_entry *a = entry;
	const struct moved_entry *b = entry_or_key;
	unsigned flags = diffopt->color_moved_ws_handling
			 & XDF_WHITESPACE_FLAGS;

	if (diffopt->color_moved_ws_handling &
	    COLOR_MOVED_WS_ALLOW_INDENTATION_CHANGE)
		/*
		 * As there is not specific white space config given,
		 * we'd need to check for a new block, so ignore all
		 * white space. The setup of the white space
		 * configuration for the next block is done else where
		 */
		flags |= XDF_IGNORE_WHITESPACE;

				    flags);
	unsigned flags = o->color_moved_ws_handling & XDF_WHITESPACE_FLAGS;
	ret->ent.hash = xdiff_hash_string(l->line, l->len, flags);
static void pmb_advance_or_null(struct diff_options *o,
				struct moved_entry *match,
				struct hashmap *hm,
				struct moved_block *pmb,
				int pmb_nr)
{
	int i;
	for (i = 0; i < pmb_nr; i++) {
		struct moved_entry *prev = pmb[i].match;
		struct moved_entry *cur = (prev && prev->next_line) ?
				prev->next_line : NULL;
		if (cur && !hm->cmpfn(o, cur, match, NULL)) {
			pmb[i].match = cur;
		} else {
			pmb[i].match = NULL;
		}
	}
}

static void pmb_advance_or_null_multi_match(struct diff_options *o,
					    struct moved_entry *match,
					    struct hashmap *hm,
					    struct moved_block *pmb,
					    int pmb_nr, int n)
{
	int i;
	char *got_match = xcalloc(1, pmb_nr);

	for (; match; match = hashmap_get_next(hm, match)) {
		for (i = 0; i < pmb_nr; i++) {
			struct moved_entry *prev = pmb[i].match;
			struct moved_entry *cur = (prev && prev->next_line) ?
					prev->next_line : NULL;
			if (!cur)
				continue;
			if (!cmp_in_block_with_wsd(o, cur, match, &pmb[i], n))
				got_match[i] |= 1;
		}
	}

	for (i = 0; i < pmb_nr; i++) {
		if (got_match[i]) {
			/* Advance to the next line */
			pmb[i].match = pmb[i].match->next_line;
		} else {
			moved_block_clear(&pmb[i]);
		}
	}

	free(got_match);
}

static int shrink_potential_moved_blocks(struct moved_block *pmb,
		while (lp < pmb_nr && pmb[lp].match)
		while (rp > -1 && !pmb[rp].match)
			pmb[rp].match = NULL;
			pmb[rp].wsd.string = NULL;
	struct moved_block *pmb = NULL; /* potentially moved blocks */
			match = hashmap_get(hm, key, NULL);
			match = hashmap_get(hm, key, NULL);
			int i;

			for(i = 0; i < pmb_nr; i++)
				moved_block_clear(&pmb[i]);
		if (o->color_moved_ws_handling &
		    COLOR_MOVED_WS_ALLOW_INDENTATION_CHANGE)
			pmb_advance_or_null_multi_match(o, match, hm, pmb, pmb_nr, n);
		else
			pmb_advance_or_null(o, match, hm, pmb, pmb_nr);
				if (o->color_moved_ws_handling &
				    COLOR_MOVED_WS_ALLOW_INDENTATION_CHANGE) {
					if (compute_ws_delta(l, match->es,
							     &pmb[pmb_nr].wsd))
						pmb[pmb_nr++].match = match;
				} else {
					pmb[pmb_nr].wsd.string = NULL;
					pmb[pmb_nr++].match = match;
				}
		if (flipped_block && o->color_moved != COLOR_MOVED_BLOCKS)
	for(n = 0; n < pmb_nr; n++)
		moved_block_clear(&pmb[n]);
				const char *set_sign, const char *set,
				const char *reset,
				char sign, const char *line, int len,
	if (!ws && !set_sign)
		emit_line_0(o, set, NULL, 0, reset, sign, line, len);
	else if (!ws) {
		emit_line_0(o, set_sign, set, !!set_sign, reset, sign, line, len);
	} else if (blank_at_eof)
		emit_line_0(o, ws, NULL, 0, reset, sign, line, len);
		emit_line_0(o, set_sign ? set_sign : set, NULL, !!set_sign, reset,
			    sign, "", 0);
	const char *context, *reset, *set, *set_sign, *meta, *fraginfo;
		emit_line_0(o, context, NULL, 0, reset, '\\',
		set_sign = NULL;
		if (o->flags.dual_color_diffed_diffs) {
			char c = !len ? 0 : line[0];

			if (c == '+')
				set = diff_get_color_opt(o, DIFF_FILE_NEW);
			else if (c == '@')
				set = diff_get_color_opt(o, DIFF_FRAGINFO);
			else if (c == '-')
				set = diff_get_color_opt(o, DIFF_FILE_OLD);
		}
		emit_line_ws_markup(o, set_sign, set, reset,
				    o->output_indicators[OUTPUT_INDICATOR_CONTEXT],
				    line, len,
		if (!o->flags.dual_color_diffed_diffs)
			set_sign = NULL;
		else {
			char c = !len ? 0 : line[0];

			set_sign = set;
			if (c == '-')
				set = diff_get_color_opt(o, DIFF_FILE_OLD_BOLD);
			else if (c == '@')
				set = diff_get_color_opt(o, DIFF_FRAGINFO);
			else if (c == '+')
				set = diff_get_color_opt(o, DIFF_FILE_NEW_BOLD);
			else
				set = diff_get_color_opt(o, DIFF_CONTEXT_BOLD);
			flags &= ~DIFF_SYMBOL_CONTENT_WS_MASK;
		}
		emit_line_ws_markup(o, set_sign, set, reset,
				    o->output_indicators[OUTPUT_INDICATOR_NEW],
				    line, len,
		if (!o->flags.dual_color_diffed_diffs)
			set_sign = NULL;
		else {
			char c = !len ? 0 : line[0];

			set_sign = set;
			if (c == '+')
				set = diff_get_color_opt(o, DIFF_FILE_NEW_DIM);
			else if (c == '@')
				set = diff_get_color_opt(o, DIFF_FRAGINFO);
			else if (c == '-')
				set = diff_get_color_opt(o, DIFF_FILE_OLD_DIM);
			else
				set = diff_get_color_opt(o, DIFF_CONTEXT_DIM);
		}
		emit_line_ws_markup(o, set_sign, set, reset,
				    o->output_indicators[OUTPUT_INDICATOR_OLD],
				    line, len,
	const char *reverse = ecbdata->color_diff ? GIT_COLOR_REVERSE : "";
	if (ecbdata->opt->flags.dual_color_diffed_diffs)
		strbuf_addstr(&msgbuf, reverse);
	size_one = fill_textconv(o->repo, textconv_one, one, &data_one);
	size_two = fill_textconv(o->repo, textconv_two, two, &data_two);
	ecbdata.ws_rule = whitespace_rule(o->repo->index, name_b);
static void diff_filespec_load_driver(struct diff_filespec *one,
				      struct index_state *istate)
		one->driver = userdiff_find_by_path(istate, one->path);
static const char *userdiff_word_regex(struct diff_filespec *one,
				       struct index_state *istate)
	diff_filespec_load_driver(one, istate);
		o->word_regex = userdiff_word_regex(one, o->repo->index);
		o->word_regex = userdiff_word_regex(two, o->repo->index);
			die("invalid regular expression: %s",
			    o->word_regex);
		if (p->one->oid_valid && p->two->oid_valid &&
		    oideq(&p->one->oid, &p->two->oid)) {
			diff_populate_filespec(options->repo, p->one, 0);
			diff_populate_filespec(options->repo, p->two, 0);
			diffcore_count_changes(options->repo,
					       p->one, p->two, NULL, NULL,
			diff_populate_filespec(options->repo, p->one, CHECK_SIZE_ONLY);
			diff_populate_filespec(options->repo, p->two, CHECK_SIZE_ONLY);
		 * be identical, but since the oid changed, we
int diff_filespec_is_binary(struct repository *r,
			    struct diff_filespec *one)
		diff_filespec_load_driver(one, r->index);
				diff_populate_filespec(r, one, CHECK_BINARY);
static const struct userdiff_funcname *
diff_funcname_pattern(struct diff_options *o, struct diff_filespec *one)
	diff_filespec_load_driver(one, o->repo->index);
struct userdiff_driver *get_textconv(struct index_state *istate,
				     struct diff_filespec *one)
	diff_filespec_load_driver(one, istate);
		textconv_one = get_textconv(o->repo->index, one);
		textconv_two = get_textconv(o->repo->index, two);
		    (textconv_one || !diff_filespec_is_binary(o->repo, one)) &&
		    (textconv_two || !diff_filespec_is_binary(o->repo, two))) {
					  textconv_one, textconv_two, o);
		   ( (!textconv_one && diff_filespec_is_binary(o->repo, one)) ||
		     (!textconv_two && diff_filespec_is_binary(o->repo, two)) )) {
			if (oideq(&one->oid, &two->oid)) {
		if (fill_mmfile(o->repo, &mf1, one) < 0 ||
		    fill_mmfile(o->repo, &mf2, two) < 0)
		mf1.size = fill_textconv(o->repo, textconv_one, one, &mf1.ptr);
		mf2.size = fill_textconv(o->repo, textconv_two, two, &mf2.ptr);
		pe = diff_funcname_pattern(o, one);
			pe = diff_funcname_pattern(o, two);
		if (o->flags.suppress_diff_headers)
			lbl[0] = NULL;
		ecbdata.ws_rule = whitespace_rule(o->repo->index, name_b);
		if (header.len && !o->flags.suppress_diff_headers)
			ecbdata.header = &header;
	same_contents = oideq(&one->oid, &two->oid);
	if (diff_filespec_is_binary(o->repo, one) ||
	    diff_filespec_is_binary(o->repo, two)) {
			data->added = diff_filespec_size(o->repo, two);
			data->deleted = diff_filespec_size(o->repo, one);
		diff_populate_filespec(o->repo, one, 0);
		diff_populate_filespec(o->repo, two, 0);
		if (fill_mmfile(o->repo, &mf1, one) < 0 ||
		    fill_mmfile(o->repo, &mf2, two) < 0)
	data.ws_rule = whitespace_rule(o->repo->index, attr_path);
	data.conflict_marker_size = ll_merge_marker_size(o->repo->index, attr_path);
	if (fill_mmfile(o->repo, &mf1, one) < 0 ||
	    fill_mmfile(o->repo, &mf2, two) < 0)
	if (diff_filespec_is_binary(o->repo, two))
static int reuse_worktree_file(struct index_state *istate,
			       const char *name,
			       const struct object_id *oid,
			       int want_file)
	if (!istate->cache)
	if (!want_file && would_convert_to_git(istate, name))
	pos = index_name_pos(istate, name, len);
	ce = istate->cache[pos];
	if (!oideq(oid, &ce->oid) || !S_ISREG(ce->ce_mode))
	    (!lstat(name, &st) && !ie_match_stat(istate, ce, &st, 0)))
int diff_populate_filespec(struct repository *r,
			   struct diff_filespec *s,
			   unsigned int flags)
	    reuse_worktree_file(r->index, s->path, &s->oid, 0)) {
		if (size_only && !would_convert_to_git(r->index, s->path))
		if (convert_to_git(r->index, s->path, s->data, s->size, &buf, conv_flags)) {
			type = oid_object_info(r, &s->oid, &s->size);
static void prep_temp_blob(struct index_state *istate,
			   const char *path, struct diff_tempfile *temp,
	if (convert_to_working_tree(istate, path,
static struct diff_tempfile *prepare_temp_file(struct repository *r,
					       const char *name,
					       struct diff_filespec *one)
	     reuse_worktree_file(r->index, name, &one->oid, 1))) {
			prep_temp_blob(r->index, name, temp, sb.buf, sb.len,
		if (diff_populate_filespec(r, one, 0))
		prep_temp_blob(r->index, name, temp,
			       one->data, one->size,
static void add_external_diff_name(struct repository *r,
				   struct argv_array *argv,
	struct diff_tempfile *temp = prepare_temp_file(r, name, df);
		add_external_diff_name(o->repo, &argv, name, one);
			add_external_diff_name(o->repo, &argv, name, two);
			add_external_diff_name(o->repo, &argv, other, two);
		if (abbrev > the_hash_algo->hexsz)
	if (one && two && !oideq(&one->oid, &two->oid)) {
			if ((!fill_mmfile(o->repo, &mf, one) &&
			     diff_filespec_is_binary(o->repo, one)) ||
			    (!fill_mmfile(o->repo, &mf, two) &&
			     diff_filespec_is_binary(o->repo, two)))
		struct userdiff_driver *drv;

		drv = userdiff_find_by_path(o->repo->index, attr_path);
static void diff_fill_oid_info(struct diff_filespec *one, struct index_state *istate)
			if (index_path(istate, &one->oid, one->path, &st, 0))
	diff_fill_oid_info(one, o->repo->index);
	diff_fill_oid_info(two, o->repo->index);
			     one, null, &msg,
			     o, p);
		builtin_diffstat(p->one->path, NULL, NULL, NULL,
				 diffstat, o, p);
	diff_fill_oid_info(p->one, o->repo->index);
	diff_fill_oid_info(p->two, o->repo->index);
	builtin_diffstat(name, other, p->one, p->two,
			 diffstat, o, p);
	diff_fill_oid_info(p->one, o->repo->index);
	diff_fill_oid_info(p->two, o->repo->index);
void repo_diff_setup(struct repository *r, struct diff_options *options)
	options->repo = r;
	options->output_indicators[OUTPUT_INDICATOR_NEW] = '+';
	options->output_indicators[OUTPUT_INDICATOR_OLD] = '-';
	options->output_indicators[OUTPUT_INDICATOR_CONTEXT] = ' ';
	options->color_moved_ws_handling = diff_color_moved_ws_default;
	else if (skip_prefix(arg, "--output-indicator-new=", &arg))
		options->output_indicators[OUTPUT_INDICATOR_NEW] = arg[0];
	else if (skip_prefix(arg, "--output-indicator-old=", &arg))
		options->output_indicators[OUTPUT_INDICATOR_OLD] = arg[0];
	else if (skip_prefix(arg, "--output-indicator-context=", &arg))
		options->output_indicators[OUTPUT_INDICATOR_CONTEXT] = arg[0];
	} else if (skip_prefix(arg, "--color-moved-ws=", &arg)) {
		options->color_moved_ws_handling = parse_color_moved_ws(arg);
	if (len == the_hash_algo->hexsz)
	if (abblen < the_hash_algo->hexsz - 3) {
	    oideq(&one->oid, &two->oid) &&
		else if (!oideq(&p->one->oid, &p->two->oid) ||
		diff_fill_oid_info(p->one, options->repo->index);
		diff_fill_oid_info(p->two, options->repo->index);
		if (fill_mmfile(options->repo, &mf1, p->one) < 0 ||
		    fill_mmfile(options->repo, &mf2, p->two) < 0)
		if (diff_filespec_is_binary(options->repo, p->one) ||
		    diff_filespec_is_binary(options->repo, p->two)) {
			if (o->color_moved_ws_handling &
			    COLOR_MOVED_WS_ALLOW_INDENTATION_CHANGE)
				o->color_moved_ws_handling |= XDF_IGNORE_WHITESPACE;

			hashmap_init(&del_lines, moved_entry_cmp, o, 0);
			hashmap_init(&add_lines, moved_entry_cmp, o, 0);
			hashmap_free(&add_lines, 1);
			hashmap_free(&del_lines, 1);
static int diff_filespec_is_identical(struct repository *r,
				      struct diff_filespec *one,
	if (diff_populate_filespec(r, one, 0))
	if (diff_populate_filespec(r, two, 0))
static int diff_filespec_check_stat_unmatch(struct repository *r,
					    struct diff_filepair *p)
	    diff_populate_filespec(r, p->one, CHECK_SIZE_ONLY) ||
	    diff_populate_filespec(r, p->two, CHECK_SIZE_ONLY) ||
	    !diff_filespec_is_identical(r, p->one, p->two)) /* (2) */
		if (diff_filespec_check_stat_unmatch(diffopt->repo, p))
			diffcore_break(options->repo,
				       options->break_opt);
	    !diff_filespec_check_stat_unmatch(options->repo, p))
static char *run_textconv(struct repository *r,
			  const char *pgm,
			  struct diff_filespec *spec,
			  size_t *outsize)
	temp = prepare_temp_file(r, spec->path, spec);
size_t fill_textconv(struct repository *r,
		     struct userdiff_driver *driver,
		if (diff_populate_filespec(r, df, 0))
	*outbuf = run_textconv(r, driver->textconv, df, &size);
int textconv_object(struct repository *r,
		    const char *path,
	textconv = get_textconv(r->index, df);
	*buf_size = fill_textconv(r, textconv, df, buf);