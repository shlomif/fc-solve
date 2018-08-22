"use strict";

define([],
       function() {
           function jq_querystring(arg) {
               return $.querystring(arg);
           }
           return {jq_querystring: jq_querystring};
       });
