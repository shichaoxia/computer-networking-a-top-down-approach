EMACS := emacs
OSASCRIPT := osascript
ORGS := $(wildcard *.org)
HTMLS := $(ORGS:.org=.html)

.PHONY: html
html: $(HTMLS)

.PHONY: watch
watch:
	fswatch -o $(ORGS) | xargs -n1 -I{} make

.PHONY: livereload
livereload:
	browser-sync start --server --files="*.html"

%.html: %.org
	@echo 'generate $@ from $<'
	@$(EMACS) --batch --quick --eval '$(call orgexport,$(abspath $<))'

define orgexport
(progn\
(setq make-backup-files nil)\
(find-file "$1")\
(org-html-export-to-html))
endef
