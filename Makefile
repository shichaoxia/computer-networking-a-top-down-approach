ORGS := $(wildcard *.org)
ADOCS := $(wildcard *.adoc)

index.html: $(ADOCS)
	asciidoctor -r asciidoctor-diagram index.adoc

.PHONY: watch
watch:
	fswatch -o $(ADOCS) | xargs -n1 -I{} make

.PHONY: livereload
livereload:
	browser-sync start --server --files="*.html"
