<!DOCTYPE style-sheet PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN" [
<!ENTITY html-ss 
  PUBLIC "-//Norman Walsh//DOCUMENT DocBook HTML Stylesheet//EN" CDATA dsssl>
<!ENTITY print-ss
  PUBLIC "-//Norman Walsh//DOCUMENT DocBook Print Stylesheet//EN" CDATA dsssl>
]>
<style-sheet>
<style-specification id="print" use="print-stylesheet">
<style-specification-body> 
(define %bop-footnotes%
   ;; Make "bottom-of-page" footnotes?
   #t)

(declare-flow-object-class formatting-instruction
  "UNREGISTERED::James Clark//Flow Object Class::formatting-instruction")

(element ulink
  (let ((type (attribute-string (normalize "type"))))
    (if (equal? type "htmlonly")
      (sosofo-append
        (process-children)                ;; Wrote the text
      )
      (sosofo-append
        (make formatting-instruction  data: "\\href{" )
        (make formatting-instruction 
            data: (attribute-string (normalize "url")) )
        (make formatting-instruction  data: "}{" )
        (process-children) ;; Wrote the text with its format (HTML anchor)
        (make formatting-instruction  data: "}" ) )
    )
  )
)

(define %generate-article-toc%
    1
)

(define (toc-depth nd)
    100
)

</style-specification-body>
</style-specification>
<style-specification id="html" use="html-stylesheet">
<style-specification-body> 

;; customize the html stylesheet
(define %html-ext%
    ".html"
)

; Make sure there is a table of contents
(define %generate-article-toc%
    1
)

(define (toc-depth nd)
    100
)

</style-specification-body>
</style-specification>
<external-specification id="print-stylesheet" document="print-ss">
<external-specification id="html-stylesheet"  document="html-ss">
</style-sheet>

