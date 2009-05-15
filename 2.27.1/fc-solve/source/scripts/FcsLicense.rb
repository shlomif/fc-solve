$X11L_TEMPLATE = <<"EOF";
Copyright (c) <year> <copyright holders>

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
EOF

def license_to_comment()
    text = $X11L_TEMPLATE
    text += "\n"
    text.gsub!(/^/, " * ");
    text.sub!(/ \*/, "/*");
    text.sub!(/ \* \Z/, " */");
    text.gsub!(/^ \* +$/, " *");
    text.sub!(/<year>/, "2000");
    text.sub!(/<copyright holders>/, "Shlomi Fish");

    return text
end


def change_c_file(filename)
    outfn = filename + ".new"
    outfile = File.open(outfn, "w")
    File.open(filename, "r") do |file|
        # Preserve the leading whitespace
        while ((l = file.gets) && (l !~ /\S/))
            outfile.puts(l)
        end
        if (l =~ /^\s*\/\*/) then
            comment = ""
            comment += l
            # We have a comment.
            while ((l = file.gets) && (l !~ /Written by|This file is in the public domain/i))
                comment += l
            end
            while ((l = file.gets) && (l !~ /\*\//))
                # Do nothing - we skip the comment
                1;
            end
            outfile.puts(license_to_comment())
            outfile.puts(comment + " */")
        else
            outfile.puts(license_to_comment())
            outfile.puts(<<"EOF")
/*
 * TODO : Add a description of this file.
 */
EOF
            outfile.puts(l)
        end
        # Write the rest of the file as is.
        while (l = file.gets)
            outfile.puts(l)
        end
    end
    outfile.close
    File.rename(outfn, filename)
end

def license_to_perl_pod()
    text = $X11L_TEMPLATE
    text += "\n"
    text.sub!(/<year>/, "2000");
    text.sub!(/<copyright holders>/, "Shlomi Fish");

    return "\n\n=head1 COPYRIGHT AND LICENSE\n\n" + text + "\n\n=cut\n\n";
end

def change_perl_file(filename)
    File.open(filename, "a") do |fh|
        fh.write(license_to_perl_pod())
    end
end

