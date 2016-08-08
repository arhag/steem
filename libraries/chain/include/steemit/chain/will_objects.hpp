#pragma once

#include <steemit/chain/protocol/authority.hpp>
#include <steemit/chain/protocol/types.hpp>
#include <steemit/chain/protocol/steem_operations.hpp>

#include <graphene/db/generic_index.hpp>

#include <boost/multi_index/composite_key.hpp>

namespace steemit { namespace chain {

   using namespace graphene::db;

   /**
    *  The actual active will item within some account's will contract.
    */
   class will_item_object : public abstract_object<will_item_object>
   {
      public:
         static const uint8_t space_id = implementation_ids;
         static const uint8_t type_id  = impl_will_item_object_type;
        
         account_id_type      benefactor;
         uint16_t             will_item_id;
         authority            beneficiary_authority;
         uint32_t             waiting_period;
         uint16_t             percent;

         will_item_id_type get_id()const { return id; }
   };

   /**
    *  The inheritance object holding some amount of VESTS.
    */
   class inheritance_object : public abstract_object<inheritance_object>
   {
      public:
         static const uint8_t space_id = implementation_ids;
         static const uint8_t type_id  = impl_inheritance_object_type;
        
         uint64_t             inheritance_id;
         account_id_type      owner;
         asset                vesting_shares = asset( 0, VESTS_SYMBOL );
         time_point_sec       created;

         inheritance_id_type get_id()const { return id; }
   };

   /**
    *  Full beneficiary claim associated to a particular will item
    */
   class full_beneficiary_claim_object : public abstract_object<full_beneficiary_claim_object>
   {
      public:
         static const uint8_t space_id = implementation_ids;
         static const uint8_t type_id  = impl_full_beneficiary_claim_object_type;
       
         account_id_type      associated_benefactor; 
         will_item_id_type    associated_will_item;
         authority            new_owner_authority;
         time_point_sec       effective_on;
   };

   /**
    *  Partial beneficiary claim associated to a particular will item
    */
   class partial_beneficiary_claim_object : public abstract_object<partial_beneficiary_claim_object>
   {
      public:
         static const uint8_t space_id = implementation_ids;
         static const uint8_t type_id  = impl_partial_beneficiary_claim_object_type;
    
         account_id_type      associated_benefactor;    
         will_item_id_type    associated_will_item;
         string               claim_account;
         time_point_sec       effective_on;
   };
 
   /**
    *  The pending change to a will contract.
    */
   class change_will_object : public abstract_object<change_will_object>
   {
      public:
         static const uint8_t space_id = implementation_ids;
         static const uint8_t type_id  = impl_change_will_object_type;
        
         account_id_type      benefactor;
         uint32_t             new_active_proof_duration;
         uint32_t             new_owner_proof_duration;
         bool                 clear_will_items;
         time_point_sec       effective_on;
   };


   /**
    *  The pending change to a will item.
    */
   class change_will_item_object : public abstract_object<change_will_item_object>
   {
      public:
         static const uint8_t space_id = implementation_ids;
         static const uint8_t type_id  = impl_change_will_item_object_type;
        
         account_id_type      benefactor;
         uint16_t             will_item_id;
         authority            new_beneficiary_authority;
         uint32_t             new_waiting_period;
         uint16_t             new_percent;
         time_point_sec       effective_on;
   };

   /**
    *  The pending change of the owner of an inheritance object.
    */
   class change_inheritance_owner_object : public abstract_object<change_inheritance_owner_object>
   {
      public:
         static const uint8_t space_id = implementation_ids;
         static const uint8_t type_id  = impl_change_inheritance_owner_object_type;
        
         inheritance_id_type  inheritance_object_to_change;
         string               new_owner;
         time_point_sec       effective_on;
   };


   struct by_benefactor_will_item_id;

   typedef multi_index_container<
      will_item_object,
      indexed_by<
         ordered_unique< tag<by_id>, member< object, object_id_type, &object::id > >,
         ordered_unique< tag<by_benefactor_will_item_id>,
            composite_key< will_item_object,
               member<will_item_object, account_id_type, &will_item_object::benefactor >,
               member<will_item_object, uint16_t, &will_item_object::will_item_id >
            >
         >
      >
   > will_item_index_type;

   struct by_inheritance_id;

   typedef multi_index_container<
      inheritance_object,
      indexed_by<
         ordered_unique< tag<by_id>, member< object, object_id_type, &object::id > >,
         ordered_unique< tag<by_inheritance_id>, member< inheritance_object, uint64_t, &inheritance_object::inheritance_id > >
      >
   > inheritance_index_type;

   struct by_benefactor_will_item;
   struct by_will_item;
   struct by_benefactor_effective_date;
   struct by_effective_date;
   
   typedef multi_index_container<
      full_beneficiary_claim_object,
      indexed_by<
         ordered_unique< tag<by_id>, member< object, object_id_type, &object::id > >,
         ordered_unique< tag<by_benefactor_will_item>,
            composite_key< full_beneficiary_claim_object,
               member<full_beneficiary_claim_object, account_id_type, &full_beneficiary_claim_object::associated_benefactor >,
               member<full_beneficiary_claim_object, will_item_id_type, &full_beneficiary_claim_object::associated_will_item >
            >
         >,
         ordered_unique< tag<by_will_item>, member<full_beneficiary_claim_object, will_item_id_type, &full_beneficiary_claim_object::associated_will_item> >,
         ordered_unique< tag< by_benefactor_effective_date >,
            composite_key< full_beneficiary_claim_object,
               member<full_beneficiary_claim_object, account_id_type, &full_beneficiary_claim_object::associated_benefactor >,
               member<full_beneficiary_claim_object, time_point_sec, &full_beneficiary_claim_object::effective_on >,
               member<object, object_id_type, &object::id >
            >
         >,
         ordered_unique< tag< by_effective_date >,
            composite_key< full_beneficiary_claim_object,
               member<full_beneficiary_claim_object, time_point_sec, &full_beneficiary_claim_object::effective_on >,
               member<object, object_id_type, &object::id >
            >
         >
      >
   > full_beneficiary_claim_index_type;

   typedef multi_index_container<
      partial_beneficiary_claim_object,
      indexed_by<
         ordered_unique< tag<by_id>, member< object, object_id_type, &object::id > >,
         ordered_unique< tag<by_benefactor_will_item>,
            composite_key< partial_beneficiary_claim_object,
               member<partial_beneficiary_claim_object, account_id_type, &partial_beneficiary_claim_object::associated_benefactor >,
               member<partial_beneficiary_claim_object, will_item_id_type, &partial_beneficiary_claim_object::associated_will_item >
            >
         >,
         ordered_unique< tag<by_will_item>, member<partial_beneficiary_claim_object, will_item_id_type, &partial_beneficiary_claim_object::associated_will_item> >,
         ordered_unique< tag< by_effective_date >,
            composite_key< partial_beneficiary_claim_object,
               member<partial_beneficiary_claim_object, time_point_sec, &partial_beneficiary_claim_object::effective_on >,
               member<object, object_id_type, &object::id >
            >
         >
      >
   > partial_beneficiary_claim_index_type;

   struct by_benefactor;

   typedef multi_index_container<
      change_will_object,
      indexed_by<
         ordered_unique< tag<by_id>, member< object, object_id_type, &object::id > >,
         ordered_unique< tag<by_benefactor>, member<change_will_object, account_id_type, &change_will_object::benefactor> >,
         ordered_unique< tag< by_effective_date >,
            composite_key< change_will_object,
               member<change_will_object, time_point_sec, &change_will_object::effective_on >,
               member<object, object_id_type, &object::id >
            >
         >
      >
   > change_will_index_type;

   typedef multi_index_container<
      change_will_item_object,
      indexed_by<
         ordered_unique< tag<by_id>, member< object, object_id_type, &object::id > >,
         ordered_unique< tag<by_benefactor_will_item_id>,
            composite_key< change_will_item_object,
               member<change_will_item_object, account_id_type, &change_will_item_object::benefactor >,
               member<change_will_item_object, uint16_t, &change_will_item_object::will_item_id >
            >
         >,
         ordered_unique< tag< by_effective_date >,
            composite_key< change_will_item_object,
               member<change_will_item_object, time_point_sec, &change_will_item_object::effective_on >,
               member<object, object_id_type, &object::id >
            >
         >     
      >
   > change_will_item_index_type;

   struct by_inheritance;

   typedef multi_index_container<
      change_inheritance_owner_object,
      indexed_by<
         ordered_unique< tag<by_id>, member< object, object_id_type, &object::id > >,
         ordered_unique< tag<by_inheritance>, member<change_inheritance_owner_object, inheritance_id_type, &change_inheritance_owner_object::inheritance_object_to_change> >,
         ordered_unique< tag< by_effective_date >,
            composite_key< change_inheritance_owner_object,
               member<change_inheritance_owner_object, time_point_sec, &change_inheritance_owner_object::effective_on >,
               member<object, object_id_type, &object::id >
            >
         >
      >
   > change_inheritance_owner_index_type;


   typedef generic_index< will_item_object,                 will_item_index_type >                    will_item_index;
   typedef generic_index< inheritance_object,               inheritance_index_type >                  inheritance_index;
   typedef generic_index< full_beneficiary_claim_object,    full_beneficiary_claim_index_type >       full_beneficiary_claim_index;
   typedef generic_index< partial_beneficiary_claim_object, partial_beneficiary_claim_index_type >    partial_beneficiary_claim_index;
   typedef generic_index< change_will_object,               change_will_index_type >                  change_will_index;
   typedef generic_index< change_will_item_object,          change_will_item_index_type >             change_will_item_index;
   typedef generic_index< change_inheritance_owner_object,  change_inheritance_owner_index_type >     change_inheritance_owner_index;

} }

FC_REFLECT_DERIVED( steemit::chain::will_item_object, (graphene::db::object), 
                    (benefactor)(will_item_id)(beneficiary_authority)(waiting_period)(percent)
                  )
FC_REFLECT_DERIVED( steemit::chain::inheritance_object, (graphene::db::object), 
                    (inheritance_id)(owner)(vesting_shares)(created)
                  )
FC_REFLECT_DERIVED( steemit::chain::full_beneficiary_claim_object, (graphene::db::object), 
                    (associated_benefactor)(associated_will_item)(new_owner_authority)(effective_on)
                  )
FC_REFLECT_DERIVED( steemit::chain::partial_beneficiary_claim_object, (graphene::db::object), 
                    (associated_benefactor)(associated_will_item)(claim_account)(effective_on)
                  )
FC_REFLECT_DERIVED( steemit::chain::change_will_object, (graphene::db::object), 
                    (benefactor)(new_active_proof_duration)(new_owner_proof_duration)(clear_will_items)(effective_on)
                  )
FC_REFLECT_DERIVED( steemit::chain::change_will_item_object, (graphene::db::object), 
                    (benefactor)(will_item_id)(new_beneficiary_authority)(new_waiting_period)(new_percent)(effective_on)
                  )
FC_REFLECT_DERIVED( steemit::chain::change_inheritance_owner_object, (graphene::db::object), 
                    (inheritance_object_to_change)(new_owner)(effective_on)
                  )
