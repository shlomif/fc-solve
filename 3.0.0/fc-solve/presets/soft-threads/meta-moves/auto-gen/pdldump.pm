#  file   pdldump.pm
package PDL::Core;

my $max_elem = 10000000;  # set your max here

sub PDL::Core::string {
   my ( $self, $format ) = @_;
   if ( $PDL::_STRINGIZING ) {
      return "ALREADY_STRINGIZING_NO_LOOPS";
   }
   local $PDL::_STRINGIZING = 1;
   my $ndims = $self->getndims;
   if ( $self->nelem > $max_elem ) {
      return "TOO LONG TO PRINT";
   }
   if ( $ndims == 0 ) {
      if ( $self->badflag() and $self->isbad() ) {
         return "BAD";
      }
      else {
         my @x = $self->at();
         return ( $format ? sprintf( $format, $x[ 0 ] ) : "$x[0]" );
      }
   }
   return "Null"  if $self->isnull;
   return "Empty" if $self->isempty;    # Empty piddle

   local $PDL::Core::sep  = $PDL::use_commas ? "," : " ";
   local $PDL::Core::sep2 = $PDL::use_commas ? "," : "";
   if ( $ndims == 1 ) {
      return PDL::Core::str1D( $self, $format );
   }
   else {
      return PDL::Core::strND( $self, $format, 0 );
   }
}

1;

